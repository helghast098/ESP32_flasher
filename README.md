
# ESP32_flasher

Supported **host** microcontrollers
- ESP32 Series

Supported **target** microcontrollers
- ESP32 Series

## Description
* Use an ESP32 to flash another ESP32 through UART. Built upon the [esp-serial-flasher](https://github.com/espressif/esp-serial-flasher) library

### Configuration
Configuration toggles available to user:
* `FLASH_BAUDRATE`/`BOOTLOADER_ADDR_START`/`PARTITION_TABLE_ADDR_START`/`APP_ADDR_START`/`TX_GPIO`/`RX_GPIO`/`RESET_GPIO`/`BOOTMODE_GPIO`
* The configurations from [esp-serial-flasher](https://github.com/espressif/esp-serial-flasher) library

DEFAULT: `FLASH_BAUDRATE`
* `115200`

DEFAULT: `BOOTLOADER_ADDR_START`
* `0x1000`

DEFAULT: `PARTITION_TABLE_ADDR_START`
* `0x8000`

DEFAULT: `APP_ADDR_START`
* `0x10000`

DEFAULT: `TX_GPIO`
* `GPIO_NUM_4`

DEFAULT: `RX_GPIO`
* `GPIO_NUM_5`

DEFAULT: `RESET_GPIO`
* `GPIO_NUM_25`

DEFAULT: `BOOTMODE_GPIO`
* `GPIO_NUM_26`

## Setup
* The flasher has a default bootloader, partition, and app images (found in the binaries folder) to turn an LED on and off.
* To flash your images just replace the binaries in the binary folder `NOTE: the binaries must be named exactly as the original binary files`

## Run
* The project is ready to run, so just use `idf.py -p [device] flash monitor` to flash the device and monitor the flashed target

## LICENSE
* This project is licensed under the MIT License - see the LICENSE.md file for details
