# grblHAL driver for STM32G0xx processors

A [grblHAL](https://github.com/grblHAL) driver for the STM32G0 family (Cortex-M0+), ported from the official [STM32F1xx driver](https://github.com/grblHAL/STM32F1xx) by Terje Io.

Primary target: the [BTT SKR Mini E3 V3.0](https://github.com/bigtreetech/BIGTREETECH-SKR-mini-E3) 3D printer board (STM32G0B1RET6), a cheap and widely available drop-in CNC controller with onboard TMC2209 drivers.

## Status

Verified on hardware (BTT SKR Mini E3 V3.0):

- Stepper motion (TIM-based step generation)
- TMC2209 drivers over single-wire UART (USART4)
- Native USB serial (CDC, `STM32 Virtual ComPort`)
- UART serial on the TFT header (USART2, PA2/PA3)
- Settings storage in flash (last 2K page)
- Hard limit inputs, control inputs, probe input
- Spindle PWM (SERVOS connector, PA1/TIM2_CH2) and switched outputs (FAN/HE/HB MOSFETs)
- PS-ON power supply control as an aux output (`M64`/`M65`)

Not (yet) supported: SD card, I2C/EEPROM, keypad plugin over I2C.

## Building

With [PlatformIO](https://platformio.org):

```sh
pio run -e BTT_SKR_MINI_E3_V30_USB   # native USB serial (recommended)
pio run -e BTT_SKR_MINI_E3_V30       # UART serial on the TFT header
```

Driver options are documented in [`Inc/my_machine.h`](Inc/my_machine.h). Machine-specific build environments can be kept out of git in a `platformio_local.ini` file.

## Flashing

The firmware links at `0x08002000`, preserving the stock BTT SD-card bootloader. Two options:

- **SD card**: copy `.pio/build/<env>/firmware.bin` to a FAT32 card, insert, power cycle. The bootloader renames it to `FIRMWARE.CUR` when done.
- **ST-Link (SWD)**: `pio run -e <env> -t upload`. When using `st-flash` directly, note that the binary must be padded to a multiple of 8 bytes (STM32G0 flash writes are double-word).

Back up the stock firmware first if you may want to return to 3D printing:

```sh
st-flash read stock_backup.bin 0x8000000 0x80000
```

## Board notes (SKR Mini E3 V3.0)

| Function | Connector | MCU pin |
|---|---|---|
| Serial (default) | USB-C | USB_DRD_FS |
| Serial (alt) | TFT header | PA2/PA3 (USART2) |
| Limits X/Y/Z | X/Y/Z-STOP | PC0/PC1/PC2 |
| Probe | PROBE | PC14 |
| Spindle PWM | SERVOS | PA1 (TIM2_CH2, 3.3V logic) |
| Spindle enable / direction | FAN1 / FAN0 | PC7 / PC6 (switched) |
| Coolant flood / mist | HE0 / HB | PC8 / PC9 (switched) |
| Reset/E-stop | E0-STOP | PC15 |
| Feed hold | PWR-DET | PC12 |
| Cycle start | EXP1 pin 1 | PB5 |
| PS-ON | PS-ON | PC13 (aux out, `M64 P1`/`M65 P1`) |
| 4th axis (M3) | E0 driver | PB3/PB4/PD1 |

The FAN/HE/HB outputs are low-side MOSFET switches referenced to VIN — check voltages before wiring external electronics.

## Porting notes

Differences from the F1 driver worth knowing about:

- Cortex-M0+ has no bit-banding; GPIO access uses IDR/BSRR.
- EXTI has split rising/falling pending registers (RPR1/FPR1) and three shared vectors (EXTI0_1, EXTI2_3, EXTI4_15).
- USB CDC runs on the USB_DRD_FS peripheral, clocked from HSI48 trimmed by CRS against USB SOF.
- The TMC2209 half-duplex UART needs the RX interrupt rearmed while the TX echo drains (see [grblHAL/STM32F1xx#49](https://github.com/grblHAL/STM32F1xx/issues/49)).
- `grbl/platform.h` needs `STM32_G0_PLATFORM` detection — included here as a vendored patch, pending upstreaming to [grblHAL/core](https://github.com/grblHAL/core).

The `grbl/` core and plugin directories are currently vendored snapshots (taken from the STM32F1xx driver tree); converting them to git submodules is planned once the core patch lands upstream.

## Credits and license

Based on the [grblHAL STM32F1xx driver](https://github.com/grblHAL/STM32F1xx), copyright (c) Terje Io, itself loosely based on robomechs [6-AXIS-USBCNC-GRBL](https://github.com/robomechs/6-AXIS-USBCNC-GRBL).

Licensed under the GNU General Public License v3, see [COPYING](COPYING).

---
2026-07-16
