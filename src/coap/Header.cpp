//
// Created by Wojtek on 02.12.2018.
//

#include "Header.h"

//getters
uint8_t Header::getType() const{
    return this->type;
}

uint8_t Header::getTokenLength() const{
    return this->tokenLength;
}

uint8_t Header::getCode() const{
    return this->code;
}

uint16_t Header::getMessageId() const{
    return this->messageId;
}

//setters
void Header::setMessageId(uint16_t mid){
    this->messageId = mid;
}

//parsers
int8_t Header::parseVersion(uint8_t packetBuffer[MAX_BUFFER]){
    //reads two leftmost bites of first byte in the buffer
    //then shifts them six bites to the right
    //example:
    //  01101010
    //& 11000000
    //----------
    //  01000000 >> 6 = 00000001/binary = 1/decimal
    //and sets the version in a header
    this->version = ((packetBuffer[0] & 0xC0) >> 6);

    //if not OK
    if (this->version != 1) {
        return Enums::UNSUPPORTED_VERSION;

    //if OK
    }else{
        return 0;
    }
}

uint8_t Header::parseType(uint8_t packetBuffer[MAX_BUFFER]){
    //reads next two bites of first byte in the buffer
    //then shifts them four bites to the right
    //example:
    //  01101010
    //& 00110000
    //----------
    //  00100000 >> 4 = 00000010/binary = 2/decimal
    //and sets the type in a header
    uint8_t _type = (packetBuffer[0] & 0x30) >> 4;

    //if OK
    if(_type <= 3 && _type >= 0){
        this->type = _type;
        return 0;
    //if not OK
    }else{
        return formatResponseCode(Enums::BAD_REQUEST);
    }
}

uint8_t Header::parseTokenLength(uint8_t packetBuffer[MAX_BUFFER]){
    //reads four rightmost bites of first byte in the buffer
    //example:
    //  01101010
    //& 00001111
    //----------
    //  00001010/binary = 10/decimal
    //and sets the token length in a header
    uint8_t _tokenLength = packetBuffer[0] & 0x0F;

    //if OK
    if(_tokenLength <= 8 && _tokenLength >= 0){
        this->tokenLength = _tokenLength;
        return 0;
    //if not OK
    }else{
        return formatResponseCode(Enums::BAD_REQUEST);
    }
}

uint8_t Header::parseCode(uint8_t packetBuffer[MAX_BUFFER]){
    //reads second byte in the buffer
    //and sets the code in a header
    uint8_t _code = packetBuffer[1];

    //if OK
    if(_code <= 4 && _code >= 0){
        this->code = _code;
        return 0;
    //if not OK
    }else{
        //TODO: DELETE THESE TWO LINES AFTER TESTING
        // this->code = _code;
        // return 0;
        //TODO: SET THIS BACK AFTER TESTING
        return formatResponseCode(Enums::BAD_REQUEST);
    }
}

void Header::parseMessageId(uint8_t packetBuffer[MAX_BUFFER]){
    //reads third byte in the buffer
    //then shifts it 8 bites to the left
    //then reads fourth byte in the buffer and attaches it to previous byte
    //and set the message ID in a header
    this->messageId = ((uint16_t)packetBuffer[2] << 8) | packetBuffer[3];
}

//assemblers
void Header::assembleVersion(uint8_t *packetBuffer){
    packetBuffer[0] = 0x01 << 6;
}

void Header::assembleType(uint8_t *packetBuffer, uint8_t typ){
    packetBuffer[0] |= typ << 4;
}

void Header::assembleTokenLength(uint8_t *packetBuffer, uint8_t tkl){
    packetBuffer[0] |= tkl;
}

void Header::assembleCode(uint8_t *packetBuffer, uint8_t cod){
    packetBuffer[1] = formatResponseCode(cod);
}

void Header::assembleMessageId(uint8_t *packetBuffer, uint16_t mid){
    packetBuffer[2] = (uint8_t) ((mid & 0xFF00) >> 8);
    packetBuffer[3] = (uint8_t) (mid & 0x00FF);
}

//others

uint8_t Header::formatResponseCode(uint16_t responseCode){
    uint8_t _class = (responseCode / 100) << 5;
    uint8_t detail = responseCode % 100;

    uint8_t formattedResponseCode = _class | detail;

    return formattedResponseCode;
}

//prints header details into Serial Monitor
void Header::printHeader(){
    Serial.println(F("***Header:***"));
    Serial.print(F("  ver: "));
    Serial.println(this->version);
    Serial.print(F("  type: "));
    Serial.println(this->type);
    Serial.print(F("  tkl: "));
    Serial.println(this->tokenLength);
    Serial.print(F("  code: "));
    Serial.println(this->code);
    Serial.print(F("  mid: "));
    Serial.println(this->messageId);
}
