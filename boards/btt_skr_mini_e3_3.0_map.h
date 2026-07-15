/*
  btt_skr_mini_e3_3.0_map.h - board map for BTT SKR Mini E3 V3.0 (STM32G0B1RET6)

  Part of grblHAL

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

  Connector reference: https://github.com/bigtreetech/BIGTREETECH-SKR-mini-E3
*/

#ifndef STM32G0B1xx
#error "This board has a STM32G0B1RET6 processor, select a corresponding build!"
#endif

#if SAFETY_DOOR_ENABLE && N_AUTO_SQUARED
#error "Axis configuration is not supported when safety door is enabled!"
#elif N_ABC_MOTORS > 1
#error "Axis configuration is not supported!"
#endif

#if N_AXIS == 4
#define BOARD_NAME "BTT SKR MINI E3 V3.0 4-axis"
#else
#define BOARD_NAME "BTT SKR MINI E3 V3.0"
#endif
#define BOARD_URL "https://github.com/bigtreetech/BIGTREETECH-SKR-mini-E3"

#define SERIAL_PORT     2 // USART2: PA2 = TX, PA3 = RX (TFT header)
#define SERIAL1_PORT    4 // USART4: PC10 = TX, PC11 = RX - to Trinamic drivers
#define HAS_BOARD_INIT

#ifdef TRINAMIC_ENABLE
#undef TRINAMIC_ENABLE
#endif
#ifdef TRINAMIC_MIXED_DRIVERS
#undef TRINAMIC_MIXED_DRIVERS
#endif
#define TRINAMIC_ENABLE 2209
#define TRINAMIC_MIXED_DRIVERS 0
#define TRINAMIC_STREAM 1

// Define step pulse output pins.
#define STEP_PORT               GPIOB
#define X_STEP_PIN              13 //PB13
#define Y_STEP_PIN              10 //PB10
#define Z_STEP_PIN              0  //PB0
#define STEP_OUTMODE            GPIO_MAP

// Define step direction output pins.
#define X_DIRECTION_PORT        GPIOB
#define X_DIRECTION_PIN         12 //PB12
#define Y_DIRECTION_PORT        GPIOB
#define Y_DIRECTION_PIN         2  //PB2
#define Z_DIRECTION_PORT        GPIOC
#define Z_DIRECTION_PIN         5  //PC5
#define DIRECTION_OUTMODE       GPIO_BITBAND

// Define stepper driver enable/disable output pin.
#define X_ENABLE_PORT           GPIOB
#define X_ENABLE_PIN            14 //PB14
#define Y_ENABLE_PORT           GPIOB
#define Y_ENABLE_PIN            11 //PB11
#define Z_ENABLE_PORT           GPIOB
#define Z_ENABLE_PIN            1  //PB1

// Define homing/hard limit switch input pins.
#define LIMIT_PORT              GPIOC
#define X_LIMIT_PIN             0 //PC0 - X-STOP
#define Y_LIMIT_PIN             1 //PC1 - Y-STOP
#define Z_LIMIT_PIN             2 //PC2 - Z-STOP
#define LIMIT_INMODE            GPIO_BITBAND

// Define ganged axis or A axis step pulse and step direction output pins (E0 driver).
#if N_ABC_MOTORS == 1
#define M3_AVAILABLE
#define M3_STEP_PORT            STEP_PORT
#define M3_STEP_PIN             3 //PB3
#define M3_DIRECTION_PORT       GPIOB
#define M3_DIRECTION_PIN        4 //PB4
#define M3_ENABLE_PORT          GPIOD
#define M3_ENABLE_PIN           1 //PD1
#if N_AUTO_SQUARED && !SAFETY_DOOR_ENABLE
#define M3_LIMIT_PORT           GPIOC
#define M3_LIMIT_PIN            15 //PC15 - E0-STOP (NOTE: conflicts with the reset/e-stop input!)
#endif
#endif

#define AUXOUTPUT0_PORT         GPIOA // Spindle PWM - SERVOS connector (3.3V logic)
#define AUXOUTPUT0_PIN          1
#define AUXOUTPUT1_PORT         GPIOC // Spindle direction - FAN0 (switched output)
#define AUXOUTPUT1_PIN          6
#define AUXOUTPUT2_PORT         GPIOC // Spindle enable - FAN1 (switched output)
#define AUXOUTPUT2_PIN          7
#define AUXOUTPUT3_PORT         GPIOC // Coolant flood - HE0 (switched output)
#define AUXOUTPUT3_PIN          8
#define AUXOUTPUT4_PORT         GPIOC // Coolant mist - HB (switched output)
#define AUXOUTPUT4_PIN          9
#define AUXOUTPUT5_PORT         GPIOB // Spare - FAN2 (switched output)
#define AUXOUTPUT5_PIN          15
#define AUXOUTPUT6_PORT         GPIOC // Power supply control - PS-ON connector
#define AUXOUTPUT6_PIN          13

// Define driver spindle pins
#if DRIVER_SPINDLE_ENABLE & SPINDLE_ENA
#define SPINDLE_ENABLE_PORT     AUXOUTPUT2_PORT
#define SPINDLE_ENABLE_PIN      AUXOUTPUT2_PIN
#endif
#if DRIVER_SPINDLE_ENABLE & SPINDLE_PWM
#define SPINDLE_PWM_PORT_BASE   GPIOA_BASE
#define SPINDLE_PWM_PORT        AUXOUTPUT0_PORT
#define SPINDLE_PWM_PIN         AUXOUTPUT0_PIN // PA1 - TIM2_CH2 (SERVOS)
#endif
#if DRIVER_SPINDLE_ENABLE & SPINDLE_DIR
#define SPINDLE_DIRECTION_PORT  AUXOUTPUT1_PORT
#define SPINDLE_DIRECTION_PIN   AUXOUTPUT1_PIN
#endif

// Define flood and mist coolant enable output pins.
#if COOLANT_ENABLE & COOLANT_FLOOD
#define COOLANT_FLOOD_PORT      AUXOUTPUT3_PORT
#define COOLANT_FLOOD_PIN       AUXOUTPUT3_PIN
#endif
#if COOLANT_ENABLE & COOLANT_MIST
#define COOLANT_MIST_PORT       AUXOUTPUT4_PORT
#define COOLANT_MIST_PIN        AUXOUTPUT4_PIN
#endif

#define AUXINPUT0_PORT          GPIOC // Safety door / motor fault - SD-DET
#define AUXINPUT0_PIN           3
#if I2C_STROBE_ENABLE // NOTE: shares PA1 with the spindle PWM output - do not enable both!
#define AUXINPUT1_PORT          GPIOA // I2C strobe - SERVOS connector
#define AUXINPUT1_PIN           1
#endif
#define AUXINPUT2_PORT          GPIOC // Probe - PROBE connector
#define AUXINPUT2_PIN           14
#define AUXINPUT3_PORT          GPIOC // Reset/EStop - E0-STOP connector
#define AUXINPUT3_PIN           15
#define AUXINPUT4_PORT          GPIOC // Feed hold - PWR-DET connector
#define AUXINPUT4_PIN           12
// NOTE: EXP1 pins (PB5/PA15/PA9/PA10/PB9/PB8/PD6) are reserved for the Ender 3 LCD encoder.

// Define user-control controls (cycle start, reset, feed hold) input pins.
#if CONTROL_ENABLE & CONTROL_HALT
#define RESET_PORT              AUXINPUT3_PORT
#define RESET_PIN               AUXINPUT3_PIN
#endif
#if CONTROL_ENABLE & CONTROL_FEED_HOLD
#define FEED_HOLD_PORT          AUXINPUT4_PORT
#define FEED_HOLD_PIN           AUXINPUT4_PIN
#endif


#if PROBE_ENABLE
#define PROBE_PORT              AUXINPUT2_PORT
#define PROBE_PIN               AUXINPUT2_PIN
#endif

#if I2C_STROBE_ENABLE
#define I2C_STROBE_PORT         AUXINPUT1_PORT
#define I2C_STROBE_PIN          AUXINPUT1_PIN
#endif

#if SAFETY_DOOR_ENABLE
#define SAFETY_DOOR_PORT        AUXINPUT0_PORT
#define SAFETY_DOOR_PIN         AUXINPUT0_PIN
#elif MOTOR_FAULT_ENABLE
#define MOTOR_FAULT_PORT        AUXINPUT0_PORT
#define MOTOR_FAULT_PIN         AUXINPUT0_PIN
#endif
