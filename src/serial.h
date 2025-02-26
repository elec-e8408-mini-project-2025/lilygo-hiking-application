#ifndef SERIAL_H
#define SERIAL_H
#include "Arduino.h"

#define SERIAL_BUFFER_SIZE 256

typedef enum {
    SER_READY,
    SER_NOT_READY,
    SER_ERROR
} serialStatus;

typedef struct {
    char * buf;
    int bufLen;
    serialStatus status;
} serialBuffer;

void initSerial();

serialBuffer handleSerialByte();

void writeSerialString(char * data);

void writeSerial(char * data, int dataLen);

void writeSerialRTCTime(RTC_Date date);

#endif