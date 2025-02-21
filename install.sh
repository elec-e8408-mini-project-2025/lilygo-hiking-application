#!/bin/bash


. config.ini
 
# Global variables
ARDUINO_CLI_MIN="1.1"
ARDUINO_ESP32_MIN="2.0"
# Set defaults
if [ -z "$ESP32_USB" ]; then
    ESP32_USB="/dev/ttyUSB0"
fi

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
    ARDUINO_CLI_TEST=$(which $ARDUINO_CLI_BIN)
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: arduino-cli is not installed or in PATH"
        echo -e "$INFO: Download arduino-cli in configured path: $ARDUINO_CLI_BIN? (y/n)"
        read
        if [[ $REPLY == "y" ]]; then 
            mkdir tmp
            wget --directory-prefix=tmp/ https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
            tar -xvzf tmp/arduino-cli*.tar.gz -C tmp/
            cp tmp/arduino-cli $ARDUINO_CLI_BIN
        else
            echo -e "$ERROR: arduino-cli is not installed or in PATH"
            echo -e "$ERROR:    please install arduino-cli: https://arduino.github.io/arduino-cli/1.1/"
            echo "$FIN_ERROR"
            exit 1
        fi
    fi
    # Get arduino-cli version
    ARDUINO_CLI_VERSION=$($ARDUINO_CLI_BIN version 2>&1 | awk 'FNR==1{print $3}' )
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: arduino-cli is not installed or in PATH"
        echo -e "$ERROR:    please install arduino-cli: https://arduino.github.io/arduino-cli/1.1/"
        echo "$FIN_ERROR"
        exit 1
    fi

    # Extract major and minor version
    ARDUINO_CLI_CURRENT=$(echo "$ARDUINO_CLI_VERSION" | cut -c 1-3 | tr -d .)

    if [[ ARDUINO_CLI_CURRENT -lt $(echo "$ARDUINO_CLI_MIN" | tr -d .) ]]; then
        echo $ARDUINO_CLI_CURRENT
        echo -e "$ERROR arduino-cli version must be $ARDUINO_CLI_MIN<"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO Arduino-cli version validated. Using version $ARDUINO_CLI_VERSION"
}

check_arduino_cli_esp32()
{
    # Get arduino-cli version
    ESP32_VERSION=$($ARDUINO_CLI_BIN core list 2>&1 | grep -i esp32:esp32 | awk 'FNR==1{print $2}' )
    if [[ -z $ESP32_VERSION ]]; then
        echo -e "$ERROR: arduino-cli esp32 is not installed"
        echo -e "$INFO: install esp32:esp32 core libraries? (y/n)"
        read
        if [[ $REPLY == "y" ]]; then 
            $ARDUINO_CLI_BIN core update-index --config-file arduino-cli.yaml
            $ARDUINO_CLI_BIN core install esp32:esp32@2.0.14
            python3 -m pip install pyserial
            
        else
            echo -e "$ERROR: Please run the commands: "
            echo -e "$ERROR:    arduino-cli core update-index --config-file arduino-cli.yaml"
            echo -e "$ERROR:    arduino-cli core install esp32:esp32@2.0.14"
            echo "$FIN_ERROR"
            exit 1
        fi
    fi

    # Extract major and minor version
    ESP32_CURRENT=$(echo "$ESP32_VERSION" | cut -c 1-3 | tr -d .)

    if [[ ESP32_CURRENT -lt $(echo "$ARDUINO_ESP32_MIN" | tr -d .) ]]; then
        echo -e "$ERROR arduino-cli esp32 version must be $ARDUINO_ESP32_MIN<"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO Arduino-cli esp32:esp32 library version validated. Using version $ESP32_VERSION"
}

check_usb()
{
    USB=$($ARDUINO_CLI_BIN board list | grep $ESP32_USB)
    if [ -z "$USB" ]; then
        echo -e "$ERROR USB device not found: $ESP32_USB"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO USB recognized: $ESP32_USB"
}

compile_app()
{
    echo -e "$INFO Compiling application. This may take awhile"
    case "$DEVICE" in
        ESP32_WROOM_32)
            FQBN="esp32:esp32:esp32-poe-iso"
            ;;
        LILYGO_WATCH_2020_V1)
            FQBN="esp32:esp32:twatch" 
            ;;  
        LILYGO_WATCH_2020_V2)
            FQBN="esp32:esp32:twatch" 
            ;;  
        LILYGO_WATCH_2020_V3)
            FQBN="esp32:esp32:twatch"
            ;;  
        *)
            echo -e "$ERROR: Unrecognized device: $DEVICE"
            echo "$FIN_ERROR"
            exit 1
            ;;
    esac

    $ARDUINO_CLI_BIN compile --fqbn $FQBN --build-path $(pwd)/build --build-property "build.extra_flags=-D $DEVICE -D ESP32" .
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: Compilation failed..."
        echo -e "$ERROR:    Better luck next time."
        echo -e "$FIN_ERROR"
        exit 1
    fi
    echo -e "$INFO App compiled in build/"
    $ARDUINO_CLI_BIN upload -p $ESP32_USB --fqbn $FQBN --input-dir $(pwd)/build .
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: Upload failed"
        echo -e "$ERROR:    Most likely you don't have write access."
        echo -e "$ERROR:    Add write access with command: sudo chmod 777 $ESP32_USB"
        echo -e "$FIN_ERROR"
        exit 1
    fi
    echo -e "$INFO App uploaded to $ESP32_USB"

}

install_libraries()
{
    if [ -n "$($ARDUINO_CLI_BIN lib list | grep "TTGO TWatch Library")" ]; then
        echo -e "$INFO Libraries already installed."
        return 0
    fi

    $ARDUINO_CLI_BIN config set library.enable_unsafe_install true

    $ARDUINO_CLI_BIN lib install --git-url https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library.git
    
    if [[ $? -ne 0 ]]; then
        echo -e "$ERROR: Error extracting libraries"
        echo -e "$FIN_ERROR"
        exit 1
    fi

    echo -e "$INFO TTGO TWatch library installed"
}

end_info()
{
    echo -e "$INFO To access esp32:"
    echo -e "$INFO  picocom -b 115200 $ESP32_USB"
    echo -e "$INFO  screen $ESP32_USB 115200"
    echo -e "$INFO  Or use putty..."
}

echo -e "$TITL"

# Run functions sequentially
check_arduino_cli_version
check_arduino_cli_esp32
install_libraries
compile_app
check_usb
end_info

echo -e "$FIN_INFO"
