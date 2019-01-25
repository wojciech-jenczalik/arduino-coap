//
// Created by Wojtek on 02.12.2018.
//

#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>

#include "memory/MemoryFree.h"
#include "memory/AllocationFix.h"
#include "utils/Attributes.h"
#include "utils/Enums.h"

class Header{

private:
    uint8_t version;              // Implementations of this specification MUST set this field
                                  // to 1 (01 binary).  Other values are reserved for future versions.
                                  // Messages with unknown version numbers MUST be silently ignored.

    uint8_t type;                 // Indicates if this message is of type Confirmable (0),
                                  // Non-confirmable (1), Acknowledgement (2), or Reset (3).

    uint8_t tokenLength;          // Indicates the length of
                                  // the variable-length Token field (0-8 bytes).  Lengths 9-15 are
                                  // reserved, MUST NOT be sent, and MUST be processed as a message
                                  // format error.

    uint8_t code;                 // https://tools.ietf.org/html/rfc7252#section-3
                                  // https://tools.ietf.org/html/rfc7252#section-12.1.1

    uint16_t messageId;           // 16-bit unsigned integer in network byte order.
                                  // https://www.ibm.com/support/knowledgecenter/en/SSB27U_6.4.0/com.ibm.zvm.v640.kiml0/asonetw.htm
                                  //
                                  // Used to detect message duplication and to match messages of type
                                  // Acknowledgement/Reset to messages of type Confirmable/Non-
                                  // confirmable.
                                  // https://tools.ietf.org/html/rfc7252#section-4

public:
    //getters
    uint8_t getType() const;
    uint8_t getTokenLength() const;
    uint8_t getCode() const;
    uint16_t getMessageId() const;

    //setters
    void setMessageId(uint16_t mid);

    //parsers
    int8_t   parseVersion    (uint8_t *packetBuffer);
    uint8_t  parseType       (uint8_t *packetBuffer);
    uint8_t  parseTokenLength(uint8_t *packetBuffer);
    uint8_t  parseCode       (uint8_t *packetBuffer);
    void     parseMessageId  (uint8_t *packetBuffer);

    //assemblers
    void assembleVersion    (uint8_t *sendBuffer);
    void assembleType       (uint8_t *sendBuffer, uint8_t typ);
    void assembleTokenLength(uint8_t *sendBuffer, uint8_t tkl);
    void assembleCode       (uint8_t *sendBuffer, uint8_t cod);
    void assembleMessageId  (uint8_t *sendBuffer, uint16_t mid);

    //others
    uint8_t formatResponseCode(uint16_t responseCode);
    void printHeader();
};

#endif //HEADER_H
