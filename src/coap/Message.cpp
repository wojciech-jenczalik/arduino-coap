//
// Created by Wojtek on 02.12.2018.
//

#include "Message.h"

//setters

void Message::setHeader(Header head){
    this->header = head;
}

void Message::setToken(uint64_t tok){
    this->token = tok;
}

void Message::setOption(Option opt){
    this->option = opt;
}

//getters
Header Message::getHeader() const{
    return this->header;
}

uint64_t Message::getToken() const{
    return this->token;
}

uint8_t Message::getOptionCounter() const{
    return this->optionCounter;
}

Option *Message::getOptions() const{
    return this->options;
}

//parsers
uint8_t Message::parseHeader(uint8_t *packetBuffer){
    uint8_t errorCatcher;

    errorCatcher = this->header.parseVersion(packetBuffer);
    if(errorCatcher) return errorCatcher;

    errorCatcher = this->header.parseType(packetBuffer);
    if(errorCatcher) return errorCatcher;

    errorCatcher = this->header.parseTokenLength(packetBuffer);
    if(errorCatcher) return errorCatcher;

    errorCatcher = this->header.parseCode(packetBuffer);
    if(errorCatcher) return errorCatcher;

    this->header.parseMessageId(packetBuffer);

    //returns 0 if OK
    return 0;
}

uint8_t Message::parseToken(uint8_t *packetBuffer, Header header){

    uint8_t tkl = header.getTokenLength();
    this->token = 0x0;

    if (tkl >= 0 && tkl <= 8){
        //writes bytes from packet buffer in loop
        for(int i = 0; i < tkl; i++){
            //value '4' stands for header length
            this->token |= (packetBuffer[i+4]);
            //checks if following byte is going to parsed...
            if((i+1) < tkl){
                //...if so, shifts 8 bites to the left, to make space for incoming value
                this->token = this->token << 8;
            }
        }
        //if OK
        return 0;
    }else{
        return formatResponseCode(BAD_REQUEST);
    }
}

//parses a single option, this will be called in a loop in Message::parseOptions()
//fistByte indicates a byte where options will begin being parsed
int32_t Message::parseOption(uint8_t *packetBuffer, uint16_t firstByte){

    int8_t deltaParsingResult;
    int8_t lengthParsingResult;
    uint16_t valueParsingResult;

    //parses delta and length, saves parsers' return values
    deltaParsingResult = this->option.parseDelta(packetBuffer, firstByte);
    lengthParsingResult = this->option.parseLength(packetBuffer, firstByte);

    firstByte += deltaParsingResult;
    firstByte += lengthParsingResult;
    firstByte += OPTION_HEADER_LENGTH; // = 1;

    //if value is present...
    if(deltaParsingResult != PAYLOAD_MARKER && lengthParsingResult != PAYLOAD_MARKER){
        //...parses this value and saves return value, which is option length
        valueParsingResult = this->option.parseValue(packetBuffer, firstByte);

        //returns OPTION_HEADER_LENGTH(= 1) + extended delta + extended length + value length
        //which is total length of an option
        return OPTION_HEADER_LENGTH + deltaParsingResult + lengthParsingResult + valueParsingResult;

    //if payload marker occurs
    }else if(deltaParsingResult == PAYLOAD_MARKER && lengthParsingResult == PAYLOAD_MARKER){
        return PAYLOAD_MARKER;

    //if this happens, it means that the coap message is corrupted
    }else if(deltaParsingResult == PAYLOAD_MARKER || lengthParsingResult == PAYLOAD_MARKER){
        return -1;

    //if an unknown error has occured
    }else{
        return -2;
    }
}

//messageEnd stands for packet size
int8_t Message::parseOptions(uint8_t *packetBuffer, uint8_t messageEnd){
      uint16_t firstByte = 4 + this->header.getTokenLength();
      int32_t optionParsingResult = 0;
      //this will count an amount of options for further purposes
      this->optionCounter = 0;
      //this has a safety purpose
      //makes sure that no free() will be called without earlier allocated memory
      this->options = (Option*) malloc(0);

      for(uint8_t i = 0; i < MAX_OPTIONS; i++){
           //this always assumes that an actual option is being parsed
           //validation happens in 'if' block below
           optionParsingResult = parseOption(packetBuffer, firstByte);

           //if valid...
           if((messageEnd > firstByte) && (optionParsingResult != PAYLOAD_MARKER) && (optionParsingResult != -2) && (optionParsingResult != -3)){
              //increments option counter
                this->optionCounter++;
                //reallocates memory to an Option array, based on option counter
                this->options = (Option*) realloc(options, sizeof(Option)*optionCounter);
                //writes an option into Option array
                this->options[optionCounter-1] = this->option;
                //calculates and sets option number to freshly added option
                if(this->optionCounter == 1){
                    this->options[optionCounter-1].setNumber(this->options[optionCounter-1].getDelta());
                }else if(this->optionCounter > 1){
                    this->options[optionCounter-1].setNumber(this->options[optionCounter-1].getDelta() + this->options[optionCounter-2].getNumber());
                }
                //adds option length to a firstByte indicator
                firstByte += optionParsingResult;

            }else if(optionParsingResult == PAYLOAD_MARKER){
                //returns a byte where payload parsing will begin
                return firstByte + PAYLOAD_MARKER_LENGTH;//( = 1);

            //if reached end of a message(and no payload is present)
            }else if(messageEnd <= firstByte){
                  //frees dynamically allocated memory
                  this->option.freeMemory();
                  return 0;

            //if incorrect request is detected
            }else if(optionParsingResult == -1){
                return -1;

            //unknown error has occured
            }else{
                return -2;
            }
      }
}

//messageEnd stands for packet size
uint8_t Message::parsePayload(uint8_t *packetBuffer, int16_t firstByte, uint8_t messageEnd){
    //if(no error code occured)
    if(firstByte > 0){
        uint16_t length = messageEnd - firstByte;
        //allocates memory to an array, based on payload length
        this->payload = (uint8_t*) malloc(sizeof(uint8_t)*length);

        //copies paylad values from a buffer into an array
        for(uint16_t i = 0; i < length; i++){
            this->payload[i] = packetBuffer[firstByte + i];
        }

        return 0;

    //if error code occured
    }else if(firstByte == -1){
        return formatResponseCode(BAD_REQUEST);
    }else if(firstByte == -2){
        return formatResponseCode(INTERNAL_SERVER_ERROR);
    }
}

int8_t Message::parse(uint8_t *packetBuffer, uint16_t packetSize){
    int16_t errorCatcher = 0;

    errorCatcher = this->parseHeader(packetBuffer);
    if(errorCatcher == UNSUPPORTED_VERSION){
        return -11;
    }else if(errorCatcher == 0){
        //do nothing
    }else{
        return -12;
    }

    errorCatcher = this->parseToken(packetBuffer, this->header);
    if(errorCatcher){
        return -21;
    }

    int16_t payloadFirstByte = this->parseOptions(packetBuffer, packetSize);
    if(payloadFirstByte == -1){
        return -31;
    }else if(payloadFirstByte == -2){
        return -32;
    }

    errorCatcher = this->parsePayload(packetBuffer, payloadFirstByte, packetSize);
    if(errorCatcher == formatResponseCode(BAD_REQUEST)){
        return -41;
    }else if(errorCatcher == formatResponseCode(INTERNAL_SERVER_ERROR)){
        return -42;
    }

    this->print(payloadFirstByte, packetSize);

    if((int32_t) packetSize > payloadFirstByte){
        return 1;
    }else{
        return 0;
    }
}

//assemblers

void Message::assembleHeader(uint8_t *sendBuffer, uint8_t typ, uint8_t cod){

  this->header.assembleVersion(sendBuffer);
  this->header.assembleType(sendBuffer, typ);
  this->header.assembleTokenLength(sendBuffer, this->header.getTokenLength());
  this->header.assembleCode(sendBuffer, cod);
  this->header.assembleMessageId(sendBuffer, this->header.getMessageId());
}

void Message::assembleToken(uint8_t *sendBuffer){
    uint8_t tokenLength = this->header.getTokenLength();

    for(uint8_t i = 0; i < tokenLength; i++){
        sendBuffer[4 + i] = (uint8_t) ((this->token >> (8 * (tokenLength - 1 - i))) & 0x00000000000000FF);
    }
}

void Message::addOption(uint16_t number, const char *val){
    uint16_t length = strlen_P(val);

    this->option.setNumber(number);
    this->option.setLength(length);
    this->option.setValue(val);

    if(this->optionCounter == 0){
        this->options = (Option*) malloc(0);
        this->option.setDelta(number);
    }

    this->optionCounter++;

    this->options = (Option*) realloc(this->options, sizeof(Option)*optionCounter);
    this->options[optionCounter-1] = this->option;
}

void Message::assembleOption(uint8_t *packetBuffer, Option opt, uint16_t firstByte){
    uint16_t delta = opt.getDelta();
    uint16_t length = opt.getLength();
    uint8_t *value = opt.getValue();
    uint8_t deltaExtension = 0;
    uint8_t lengthExtension = 0;

    if(delta >= 269){
        deltaExtension += 2;

        packetBuffer[firstByte] = 14 << 4;
        packetBuffer[firstByte + 1] = (delta - 269) / 256;
        packetBuffer[firstByte + 2] = (delta - 269) % 256;

    }else if(delta >= 13 && delta <= 269){
        deltaExtension += 1;

        packetBuffer[firstByte] = (0x0F & 13) << 4;
        packetBuffer[firstByte + 1] = (uint8_t) delta - 13;

    }else{
        packetBuffer[firstByte] = (0x0F & (uint8_t)delta) << 4;
    }

    if(length >= 269){
        lengthExtension += 2;

        packetBuffer[firstByte] |= (0x0F & 14);
        packetBuffer[firstByte + deltaExtension + 1] = (length - 269) / 256;
        packetBuffer[firstByte + deltaExtension + 2] = (length - 269) % 256;

    }else if(length >= 13 && length <= 269){
        lengthExtension += 1;

        packetBuffer[firstByte] |= (0x0F & 13);
        packetBuffer[firstByte + deltaExtension + 1] = (uint8_t) length - 13;

    }else{
        packetBuffer[firstByte] |= (0x0F & (uint8_t) length);
    }

    firstByte += OPTION_HEADER_LENGTH + deltaExtension + lengthExtension;

    for(uint16_t i = 0; i < length; i++){
        packetBuffer[firstByte + i] = value[i];
    }
}


uint16_t Message::assembleOptions(uint8_t *packetBuffer){

uint16_t firstByte = 4 + this->header.getTokenLength();
uint16_t delta;
uint16_t length;

    for(uint8_t i = 0; i < optionCounter; i++){
        if(i > 0){
            this->options[i].setDelta(this->options[i].getNumber() - this->options[i-1].getNumber());
        }

        delta = this->options[i].getDelta();
        length = this->options[i].getLength();

        assembleOption(packetBuffer, this->options[i], firstByte);

        firstByte += OPTION_HEADER_LENGTH + this->options[i].getLength();

        if(delta >= 269){
            firstByte += 2;

        }else if(delta >= 13 && delta <= 269){
            firstByte += 1;
        }

        if(length >= 269){
            firstByte += 2;

        }else if(length >= 13 && length <= 269){
            firstByte += 1;
        }

        this->options[i].freeMemory();

    }

    if(optionCounter > 0){
        free(this->options);
        fix_malloc_bug();
        this->options = NULL;
        this->optionCounter = 0;
    }

    return firstByte;
}

void Message::assemblePayload(uint8_t *packetBuffer, const char *payload, uint16_t firstByte){

    if(strlen_P(payload) > 0){
        packetBuffer[firstByte] = 0xFF;
        firstByte++;
    }

    for(uint16_t i = 0; i < strlen_P(payload); i++){
        packetBuffer[firstByte + i] = pgm_read_byte_near(payload + i);
    }

    for(uint16_t i = strlen_P(payload); i < MAX_BUFFER; i++){
        packetBuffer[firstByte + i] = 0;
    }
}

uint16_t Message::assemble(uint8_t *packetBuffer, uint8_t typ, uint8_t cod, const char *payload){
    this->assembleHeader(packetBuffer, typ, cod);
    this->assembleToken(packetBuffer);
    uint16_t firstByte =
    this->assembleOptions(packetBuffer);
    this->assemblePayload(packetBuffer, payload, firstByte);

    uint16_t payloadLength = strlen_P(payload);

    Serial.println(F("send buffer"));
    for(int i = 0; i < firstByte + payloadLength + 1; i++){
      Serial.print(packetBuffer[i], HEX); Serial.print(F("; "));
    }
    Serial.println(F(" "));
    Serial.println(F("/send buffer"));

    return firstByte + payloadLength;
}

//others

uint8_t Message::formatResponseCode(uint16_t responseCode){
    uint8_t _class = (responseCode / 100) << 5;
    uint8_t detail = responseCode % 100;

    uint8_t formattedResponseCode = _class | detail;

    return formattedResponseCode;
}

void Message::freeMemory(uint8_t parsingResult){

    //frees memory of each option in options array
    for(uint8_t i = 0; i < this->optionCounter; i++){
        this->options[i].freeMemory();
    }

    //frees memory of options array
    free(this->options);
    fix_malloc_bug();
    this->options = NULL;
    this->optionCounter = 0;

    //if payload is present, frees memory of a payload array
    if(parsingResult == 1){
        free(this->payload);
        fix_malloc_bug();
        this->payload = NULL;
    }
}

//prints token details into Serial Monitor
void Message::printToken(){
    Serial.println(F("***Tok:***"));
    Serial.print(F("  "));
    Serial.println((uint32_t)token, HEX);
}

//prints each option's details into Serial Monitor
void Message::printOptions(){
    for(uint8_t i = 0; i < optionCounter; i++){
        this->options[i].print();
    }
}

//prints payload details into Serial Monitor
void Message::printPayload(int16_t payloadStart, uint8_t messageEnd){
    uint8_t length = messageEnd - payloadStart;

    Serial.println(F("***Payload:***"));
    Serial.print(F("  "));
    for(uint8_t i = 0; i < length; i++){
        Serial.print(this->payload[i], HEX); Serial.print(F("; "));
    }
    Serial.println(F(""));
}

//prints details of a whole message into Serial Monitor
void Message::print(int16_t payloadStart, uint8_t messageEnd){
    Serial.println(F(" "));
    this->header.printHeader();
    printToken();
    if(optionCounter > 0){
        printOptions();
    }
    if(payloadStart > 0){
        printPayload(payloadStart, messageEnd);
    }
}
