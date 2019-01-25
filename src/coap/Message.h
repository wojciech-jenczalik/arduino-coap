//
// Created by Wojtek on 02.12.2018.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include "memory/MemoryFree.h"
#include "Header.h"
#include "Option.h"

class Message {
private:
    Header   header;
    uint64_t token;
    uint8_t  optionCounter;
    Option   option;
    Option  *options;
    uint8_t *payload;

public:
    //setters
    void setHeader(Header head);
    void setToken(uint64_t tok);
    void setOption(Option opt);

    //getters
    Header getHeader() const;
    uint64_t getToken() const;
    uint8_t getOptionCounter() const;
    Option *getOptions() const;

    //parsers
    uint8_t parseHeader (uint8_t *packetBuffer);
    uint8_t parseToken  (uint8_t *packetBuffer, Header header);
    int32_t parseOption (uint8_t *packetBuffer, uint16_t firstByte);
    int8_t  parseOptions(uint8_t *packetBuffer, uint8_t messageEnd);
    uint8_t parsePayload(uint8_t *packetBuffer, int16_t firstByte, uint8_t messageEnd);
    int8_t  parse       (uint8_t *packetBuffer, uint16_t packetSize);

    //assemblers
    void     assembleHeader (uint8_t *packetBuffer, uint8_t typ, uint8_t cod);
    void     assembleToken  (uint8_t *packetBuffer);
    void     addOption      (uint16_t num, const char *val);
    void     assembleOption (uint8_t *packetBuffer, Option opt, uint16_t firstByte);
    uint16_t assembleOptions(uint8_t *packetBuffer);
    void     assemblePayload(uint8_t *packetBuffer, const char *val, uint16_t firstByte);
    uint16_t assemble       (uint8_t *packetBuffer, uint8_t typ, uint8_t cod, const char *payload);

    //others
    uint8_t formatResponseCode(uint16_t responseCode);
    void    freeMemory        (uint8_t parsingResult);
    void    printToken        ();
    void    printOptions      ();
    void    printPayload      (int16_t payloadStart, uint8_t messageEnd);
    void    print             (int16_t payloadStart, uint8_t messageEnd);
};

#endif //MESSAGE_H
