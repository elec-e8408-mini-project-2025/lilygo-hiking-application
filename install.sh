#!/bin/bash

# Global variables
ARDUINO_CLI_MIN="1.1"
ARDUINO_ESP32_MIN="3.1"
ESP32_DEVICE="/dev/ttyUSB0"

# Colored log statuses for console outputs 
# Bold and Red color
ERROR="\033[1;31mERROR:\033[0m"
# Bold and Blue color
INFO="\033[1;34mINFO: \033[0m"
# Bold and yellow
WARN="\033[1;33mWARN: \033[0m"

# "boolean" indicating whether errors occured
ERRORS=false

DASH_LINE="----------------------------------------------------"
TITL_LINE="             LilyGo APP INSTALLER"
INFO_LINE="             INSTALLATION FINISHED"
ERRR_LINE="   INSTALLATION FAILED. SEE OUTPUTS FOR DETAILS"
TITL="$DASH_LINE\n$TITL_LINE\n$DASH_LINE"
FIN_ERROR="$DASH_LINE\n$ERRR_LINE\n$DASH_LINE"
FIN_INFO="$DASH_LINE\n$INFO_LINE\n$DASH_LINE"

# Verify that arduino is installed and meets requirements
check_arduino_cli_version() {
    # Get arduino-cli version
    ARDUINO_CLI_VERSION=$(arduino-cli version 2>&1 | awk 'FNR==1{print $3}' )
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: arduino-cli is not installed or in PATH"
        echo -e "$ERROR:    please install arduino-cli: https://arduino.github.io/arduino-cli/1.1/"
        echo "$FIN_ERROR"
        exit 1
    fi

    # Extract major and minor version
    ARDUINO_CLI_CURRENT=$(echo "$ARDUINO_CLI_VERSION" | cut -c 1-3 | tr -d .)

    if [[ ARDUINO_CLI_CURRENT -lt $(echo "$ARDUINO_CLI_MIN" | tr -d .) ]]; then
        echo -e "$ERROR arduino-cli version must be $ARDUINO_CLI_MIN<"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO Arduino-cli version validated. Using version $ARDUINO_CLI_VERSION"
}

check_arduino_cli_esp32()
{
    # Get arduino-cli version
    ESP32_VERSION=$(arduino-cli core list 2>&1 | grep -i esp32:esp32 | awk 'FNR==1{print $3}' )
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: arduino-cli esp32 is not installed"
        echo -e "$ERROR: Please run the commands: "
        echo -e "$ERROR:    arduino-cli core update-index --config-file arduino-cli.yaml"
        echo -e "$ERROR:    arduino-cli core install esp32:esp32"
        echo "$FIN_ERROR"
        exit 1
    fi

    # Extract major and minor version
    ESP32_CURRENT=$(echo "$ESP32_VERSION" | cut -c 1-3 | tr -d .)

    if [[ ESP32_CURRENT -lt $(echo "$ARDUINO_ESP32_MIN" | tr -d .) ]]; then
        echo -e "$ERROR arduino-cli version must be $ARDUINO_ESP32_MIN<"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO Arduino-cli esp32:esp32 library version validated. Using version $ESP32_VERSION"
}

check_usb()
{
    USB=$(arduino-cli board list | grep $ESP32_DEVICE)
    if [ -z "$USB" ]; then
        echo -e "$ERROR USB device not found: $ESP32_DEVICE"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO USB recognized: $ESP32_DEVICE"
}

compile_app()
{
    arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso --build-path $(pwd)/build .
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: Compilation error"
        echo "$FIN_ERROR"
        exit 1
    fi

    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: Compilation error"
        echo "$FIN_ERROR"
        exit 1
    fi

    arduino-cli upload -p $ESP32_DEVICE --fqbn esp32:esp32:esp32-poe-iso --input-dir $(pwd)/build .
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: Upload failed"
        echo -e "$ERROR:    Most likely you don't have write access."
        echo -e "$ERROR:    Add write access with command: sudo chmod 777 $ESP32_DEVICE"
        echo "$FIN_ERROR"
        exit 1
    fi

}

end_info()
{
    echo "To access esp32:"
    echo "  picocom -b 115200 $ESP32_DEVICE"
    echo "  screen $ESP32_DEVICE 115200"
}
echo -e "$TITL"

# Run functions sequentially
check_arduino_cli_version
check_arduino_cli_esp32
check_usb
compile_app

echo -e "$FIN_INFO"
