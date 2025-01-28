# LilyGo Hiking Application

Initial commit

## Requirements

What is required to install and run this application

## Installation

Installation instructions here

### Arduino-cli and esp32 libraries

1. Install arduino-cli:

https://arduino.github.io/arduino-cli/1.1/

2. Install esp32 libraries

```console
arduino-cli core update-index --config-file arduino-cli.yaml

arduino-cli core install esp32:esp32
```

3. Test your board

```console
arduino-cli board list

Port         Protocol Type              Board Name FQBN Core
/dev/ttyUSB0 serial   Serial Port (USB) Unknown
```

### Compilation and upload to esp32

```console
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso --build-path $(pwd)/build .
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32-poe-iso --input-dir $(pwd)/build .
```


### Debugging

Add read and write access to usb device 

```console
chmod 777 /dev/ttyUSB0
```


## Getting started

