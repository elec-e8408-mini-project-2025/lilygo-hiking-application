#ifndef ESP32_WROOM_32
#include "accelerator.h"
#include "globals.h"

/*
 * Sets up accelerator
 * Code taken from TTGO example stepCount
 */
void initAccelerator()
{
    Serial.println("setupAccelerator.BEGIN");
    sensor = ttgo->bma;

    // Accel parameter structure
    Acfg cfg;
    /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use steps, you must first enable the accelerometer
    // Warning : Need to use steps, you must first enable the accelerometer
    // Warning : Need to use steps, you must first enable the accelerometer
    sensor->enableAccel();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, []
                    {
          // Set interrupt to set irq value to 1
          irqAcc = 1; }, RISING); // It must be a rising edge

    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, true);

    // Reset steps
    sensor->resetStepCounter();

    // Turn on step interrupt
    sensor->enableStepCountInterrupt();
    Serial.println("setupAccelerator.END");
}

/*
 * loop handler for accelerator
 * Code taken from TTGO example stepCount
 */
void handleTasksAccelerator()
{

    // Serial.println("loopAccelerator.BEGIN");
    if (irqAcc && hasActiveSession)
    {
        Serial.println("Accelerator interrupt");
        irqAcc = 0;
        bool rlst;
        do
        {
            // Read the BMA423 interrupt status,
            // need to wait for it to return to true before continuing
            rlst = sensor->readInterrupt();
        } while (!rlst);

        // Check if it is a step interrupt
        if (sensor->isStepCounter())
        {
            // Get step data from register
            stepCount = sensor->getCounter();
        }
        // Print step count to serial
        Serial.print("Step Count: ");
        Serial.println(stepCount);
    }
    // Serial.println("loopAccelerator.END");
}

#endif