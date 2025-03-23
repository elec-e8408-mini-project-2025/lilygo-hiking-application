# LilyGo Hiking Application

LilyGO T-Watch Hiking application is part of Hiking Band System, together with the Hiking Tour Assistant Data Storage. This project provides an application for the LilyGO T-Watch smartwatch. 


## How to get started

This project has been released together with the Hiking Tour Assistant Data Storage. Within the final release are documentation to help get started. The final release has been provided as a compressed archive as a course submission. 

## Requirements

The smartwatch development has the following dependencies

- arduino-cli v. 1.1
- esp32 libraries v. 2.0.14
- python 3.10 or greater
- pyserial 3.5

## Installation

Installation instructions here

### Arduino-cli and esp32 libraries

1. Install arduino-cli:

https://arduino.github.io/arduino-cli/1.1/

2. Install esp32 libraries

```console
arduino-cli core update-index --config-file arduino-cli.yaml

arduino-cli core install esp32:esp32@2.0.14

python3 -m pip install pyserial
```

3. Test your board

```console
arduino-cli board list

Port         Protocol Type              Board Name FQBN Core
/dev/ttyUSB0 serial   Serial Port (USB) Unknown
```

### Compilation and upload to esp32

Use the following table to make your compilation:

|  Device                |    Board/FQBN              |
| ---------------------- | -------------------------- |
| ESP32_WROOM_32         |  esp32:esp32:esp32-poe-iso |
| LILYGO_WATCH_2020_V2   |  esp32:esp32:twatch        |
| LILYGO_WATCH_2020_V3   |  esp32:esp32:twatch        |

For example for TWATCH V3:

```console
DEVICE="LILYGO_WATCH_2020_V3"
FQBN=esp32:esp32:twatch
arduino-cli compile --fqbn $FQBN --build-path $(pwd)/build --build-property "build.extra_flags=-D $DEVICE -D ESP32" .
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32-poe-iso --input-dir $(pwd)/build .
```

or 

configure config.ini 

```console
./install.sh
```


### Debugging

Add read and write access to usb device:

```console
chmod 777 /dev/ttyUSB0
```

Read the serial:

```console
picocom -b 115200 /dev/ttyUSB0
or
putty
or
screen /dev/ttyUSB0 115200
```

# Important: releases

Arduino expects directory for main `.ino` file to have the same name as the `.ino` -file. For Github releases the tag is appended to the end of the directory name as a suffix. Please rename the directory to match the `.ino` file before installation, i.e. the directory should be `lilygo-hiking-application`
