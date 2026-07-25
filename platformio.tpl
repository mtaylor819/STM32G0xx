#
# Template for webbuilder
#
[platformio]
include_dir = Inc
src_dir = Src

[common]
build_flags =
  -I .
  -I boards
  -I Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
  -I Middlewares/ST/STM32_USB_Device_Library/Core/Inc
  -I USB_DEVICE/Target
  -D OVERRIDE_MY_MACHINE
build_src_filter =
  +<*>
  -<i2c.c>
  -<diskio.c>
lib_deps =
  boards
  grbl
  keypad
  motors
  trinamic
  odometer
  plugins
  Core
  Class
  App
  Target
lib_extra_dirs =
  .
  Middlewares/ST/STM32_USB_Device_Library
  USB_DEVICE

[env]
platform = ststm32
framework = stm32cube
# Do not produce .a files for lib deps (which would prevent them from overriding weak symbols)
lib_archive = no
lib_ldf_mode = off

[env:%env_name%]
board = %board%
board_build.ldscript = %ldscript%
build_flags = ${common.build_flags}
%build_flags%
build_src_filter = ${common.build_src_filter}
lib_deps = ${common.lib_deps}
%lib_deps%
lib_extra_dirs = ${common.lib_extra_dirs}
