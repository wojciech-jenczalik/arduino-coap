//
// Created by Wojtek on 07.12.2018.
//

#ifndef COAP_OPTION_H
#define COAP_OPTION_H

#include <stdint.h>

#include "memory/MemoryFree.h"
#include "memory/AllocationFix.h"
#include "utils/Attributes.h"
#include "utils/Enums.h"

using namespace Enums;

class Option {

private:
    uint16_t number;
    uint16_t delta;
    uint16_t length;
    uint8_t *value;

public:
    //getters
    uint16_t getNumber();
    uint16_t getDelta();
    uint16_t getLength();
    uint8_t* getValue();

    //setters
    void setNumber(uint16_t num);
    void setDelta (uint16_t del);
    void setLength(uint16_t len);
    void setValue (const char *val);

    //parsers
    int8_t   parseDelta (uint8_t *packetBuffer, uint16_t firstByte);
    int8_t   parseLength(uint8_t *packetBuffer, uint16_t firstByte);
    uint16_t parseValue (uint8_t *packetBuffer, uint16_t firstByte);

    //assemblers


    //others
    boolean compareValue(const char *val);
    void print();
    void freeMemory();
};

#endif //COAP_OPTION_H
