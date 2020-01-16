# Heatseek Feather Sensor

## Version Notes

### Description

This is a sensor firmware version intended for the Particle Boron LTE. It should still be back-compatible with other boards, since all Particle-specific code is wrapped in user-defined configuration flags. The following list will be updated if back-compatibility has been confirmed with each individual board:

* N/A, TODO

The flag used for Particle-specific code is `#define HEATSEEK_BORON_LTE` in `user_config.h`. By searching for this flag in the code, you should be able to find all modifications. Some notable changes are:

- The SD and DHT libraries had to be replaced with Particle-specific libraries, which have different APIs. The RTC library was also replaced, but there were no API changes.
- A new `_transmit()` function in `transmit.cpp` using Particle Webhooks.
- The DHT library used does not support heat index calculation. This is just a function, so it could be implemented if needed.

Particle LTE currently works in North America.

### Contributors
- Jake Lee (@jakehlee), Primary Contact (jake.h.lee@columbia.edu)
- Julia Guo (@julia-guo)
- Shruti Verma (@ShrutiVerma17)

## Building Project

1. This code was written, compiled, and flashed with the Particle Workbench and Visual Studio Code 1.41.1. Download it [here](https://www.particle.io/workbench/)
2. Install the required libraries via the workbench's `Install Library` command:
  - SdFat=1.0.16
  - Adafruit_RTClib_RK=1.2.1
  - PietteTech_DHT=0.0.12
3. Confirm `user_config.h` has only `#define HEATSEEK_BORON_LTE` uncommented.
4. Configure workbench for device with `Configure for device`, using the latest DeviceOS.
5. Local Compile with `Local Compile`
6. Local Flash with `Local Flash` (given the sensor is connected)
7. For deployment to multiple devices, all devices can be flashed over-the-air with `Cloud Compile` and `Cloud Flash`. 

## Software

### Outline

1. Prompt user to set RTC clock time if necessary.  (TODO: set this by web request).
1. If last reading time exceeds reading interval, take a new reading.
1. Store the reading on the SD card.
1. Store the reading time on SD card for comparison against reading interval.

### Notes

- Always prioritize logging data to SD card.  The microprocessor should always reboot and continue taking readings if there is a problem transmitting the data.
- TODO: Ensure device is not on battery power prior to writing to SD card.

## Hardware

### Base

- Heatseek Custom FeatherWing
- 2GB+ SD Card
- [DHT22 Temp/humidity sensor](https://www.adafruit.com/product/385)
- [Adalogger FeatherWing](https://www.adafruit.com/product/2922)
- [500mAh lipo battery](https://www.adafruit.com/product/1578)
- [CR1220 battery](https://www.adafruit.com/product/380)
- [MicroUSB power adapter](https://www.adafruit.com/product/1995)
- 10K ohm resistor
- Male/Female Headers (cut to size)

### WICED WiFi

- All base parts
- [WICED WiFi Feather](https://www.adafruit.com/product/3056)

### M0 WiFi

- All base parts
- [M0 WiFi Feather](https://www.adafruit.com/product/3010)

### Cellular/GSM(2G)

- All base parts
- [M0 Feather](https://www.adafruit.com/product/2772)
- [Sticker Antenna](https://www.adafruit.com/product/3237)
- [SIM800L GSM Breakout Module](http://www.ebay.com/itm/SIM800L-Quad-band-Network-Mini-GPRS-GSM-Breakout-Module-Ships-from-California-/172265821650?hash=item281bd7d5d2:g:97gAAOSwls5Y5qFG)

### Particle Boron LTE

- All base parts
- [Boron LTE w/ Antenna](https://store.particle.io/collections/cellular/products/boron-lte)

### Headseek Featherwing Board

https://oshpark.com/shared_projects/iJOnNry7

TODO: Upload schematics
