/*

  btt_skr_mini_e3_3.0.c - driver code for STM32G0xx ARM processors

  Part of grblHAL

  Copyright (c) 2021-2026 Terje Io

  grblHAL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  grblHAL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with grblHAL. If not, see <http://www.gnu.org/licenses/>.

*/

#include "driver.h"

#if defined(BOARD_BTT_SKR_MINI_E3_V30)

#include <string.h>
#include <stdio.h>

#include "serial.h"

static io_stream_t tmc_uart;

TMC_uart_write_datagram_t *tmc_uart_read (trinamic_motor_t driver, TMC_uart_read_datagram_t *dgr)
{
    static TMC_uart_write_datagram_t wdgr = {0};
    volatile uint32_t dly = 50, ms = hal.get_elapsed_ticks();

    tmc_uart.write_n(dgr->data, sizeof(TMC_uart_read_datagram_t));

    while(tmc_uart.get_tx_buffer_count());

    while(--dly);

    tmc_uart.reset_read_buffer();

    // Wait for response with 3ms timeout
    while(tmc_uart.get_rx_buffer_count() < 8) {
        if(hal.get_elapsed_ticks() - ms >= 3)
            break;
    }

    if(tmc_uart.get_rx_buffer_count() >= 8) {
        wdgr.data[0] = tmc_uart.read();
        wdgr.data[1] = tmc_uart.read();
        wdgr.data[2] = tmc_uart.read();
        wdgr.data[3] = tmc_uart.read();
        wdgr.data[4] = tmc_uart.read();
        wdgr.data[5] = tmc_uart.read();
        wdgr.data[6] = tmc_uart.read();
        wdgr.data[7] = tmc_uart.read();
    } else
        wdgr.msg.addr.value = 0xFF;

    dly = 5000;
    while(--dly);

    return &wdgr;
}

void tmc_uart_write (trinamic_motor_t driver, TMC_uart_write_datagram_t *dgr)
{
    tmc_uart.write_n(dgr->data, sizeof(TMC_uart_write_datagram_t));

    while(tmc_uart.get_tx_buffer_count());	// Wait while the datagram is delivered
}

#if LCD_ENCODER_SPINDLE

// Spindle speed control from the Ender 3 stock LCD rotary encoder (EXP1).
// Rotate: +/- RPM. Push: spindle on/off. Active in Idle and Jog states only.
//
// EXP1: BTN_EN1 = PA9, BTN_EN2 = PA10, BTN_ENC = PA15 (active low, closes to GND)

#define ENC_A_PORT GPIOA
#define ENC_A_PIN  9
#define ENC_B_PORT GPIOA
#define ENC_B_PIN  10
#define ENC_BTN_PORT GPIOA
#define ENC_BTN_PIN  15

#define ENC_RPM_STEP     500.0f
#define ENC_RPM_START    3000.0f
#define ENC_BTN_DEBOUNCE 250 // ms

static on_execute_realtime_ptr on_execute_realtime = NULL;
static float enc_rpm = 0.0f;
static bool enc_spindle_on = false;

static void encoder_poll (sys_state_t state)
{
    static uint8_t prev_ab = 0xFF;
    static int8_t pulses = 0;
    static uint32_t last_ms = 0, btn_ms = 0;
    static bool btn_prev = false;

    on_execute_realtime(state);

    uint32_t ms = hal.get_elapsed_ticks();
    if(ms == last_ms)
        return;
    last_ms = ms;

#if ENC_DEBUG
    static uint32_t dbg_ms = 0;
    if(ms - dbg_ms >= 1000) {
        dbg_ms = ms;
        char buf[48];
        sprintf(buf, "[MSG:enc A:%d B:%d BTN:%d]" ASCII_EOL,
                (int)DIGITAL_IN(ENC_A_PORT, ENC_A_PIN),
                (int)DIGITAL_IN(ENC_B_PORT, ENC_B_PIN),
                (int)DIGITAL_IN(ENC_BTN_PORT, ENC_BTN_PIN));
        hal.stream.write(buf);
    }
#endif

    // Quadrature decode, one step per detent (4 transitions)
    static const int8_t enc_delta[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

    uint8_t ab = (DIGITAL_IN(ENC_A_PORT, ENC_A_PIN) << 1) | DIGITAL_IN(ENC_B_PORT, ENC_B_PIN);

    int16_t delta = 0;

    if(prev_ab == 0xFF)
        prev_ab = ab;
    else if(ab != prev_ab) {
        pulses += enc_delta[(prev_ab << 2) | ab];
        prev_ab = ab;
        if(pulses >= 4) {
            delta = 1;
            pulses = 0;
        } else if(pulses <= -4) {
            delta = -1;
            pulses = 0;
        }
    }

    bool btn = DIGITAL_IN(ENC_BTN_PORT, ENC_BTN_PIN) == 0;
    bool btn_pressed = false;

    if(btn && !btn_prev && ms - btn_ms > ENC_BTN_DEBOUNCE) {
        btn_pressed = true;
        btn_ms = ms;
    }
    btn_prev = btn;

    if(!(delta || btn_pressed) || !(state & (STATE_IDLE|STATE_JOG)))
        return;

    if(btn_pressed) {
        enc_spindle_on = !enc_spindle_on;
        if(enc_spindle_on && enc_rpm == 0.0f)
            enc_rpm = ENC_RPM_START;
    }

    if(delta) {
        enc_rpm += (float)delta * ENC_RPM_STEP;
        if(enc_rpm < 0.0f)
            enc_rpm = 0.0f;
        if(enc_rpm > settings.pwm_spindle.rpm_max)
            enc_rpm = settings.pwm_spindle.rpm_max;
        if(enc_rpm > 0.0f && delta > 0)
            enc_spindle_on = true; // turning up from zero switches the spindle on
    }

    spindle_ptrs_t *spindle = spindle_get(0);

    if(spindle)
        spindle_set_state(spindle, (spindle_state_t){ .on = enc_spindle_on && enc_rpm > 0.0f }, enc_spindle_on ? enc_rpm : 0.0f);
}

static void encoder_init (void)
{
    GPIO_InitTypeDef GPIO_Init = {
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Pin = (1 << ENC_A_PIN)|(1 << ENC_B_PIN)|(1 << ENC_BTN_PIN)
    };
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

    on_execute_realtime = grbl.on_execute_realtime;
    grbl.on_execute_realtime = encoder_poll;
}

#endif // LCD_ENCODER_SPINDLE

void board_init (void)
{
    io_stream_t const *stream;

    if((stream = stream_open_instance(TRINAMIC_STREAM, 115200, NULL, "Trinamic UART")) == NULL)
        stream = stream_null_init(115200);

    memcpy(&tmc_uart, stream, sizeof(io_stream_t));
    tmc_uart.disable_rx(true);
    tmc_uart.set_enqueue_rt_handler(stream_buffer_all);

#if LCD_ENCODER_SPINDLE
    encoder_init();
#endif
}

#endif
