#ifndef RESTFUL_H
#define RESTFUL_H

#define MAXDATALENGTH 64

enum dataFrameType {
    GET,
    POST
};

typedef struct{
    char data[MAXDATALENGTH];
    signed int dataLength;
    dataFrameType type;
} restfulDataFrame;

























#endif