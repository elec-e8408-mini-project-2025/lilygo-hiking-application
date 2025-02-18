#!/bin/bash

# Source: https://forum.arduino.cc/t/serial-monitor-for-arduino-cli/651372

# kill all running putty processes to free the serial port
sudo killall putty

arduino-cli upload -p /dev/ttyACM0 -b arduino:avr:mini  $HOME/Arduino/lilygo-hiking-application/

# start putty
# & causes it to run in the background so it doesn't block the terminal
sudo putty /dev/ttyACM0 -serial -sercfg 115200,8,n,1,N &