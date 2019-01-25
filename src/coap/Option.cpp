//
// Created by Wojtek on 07.12.2018.
//

#include "Option.h"

//getters

uint16_t Option::getNumber(){
    return this->number;
}

uint16_t Option::getDelta(){
    return this->delta;
}

uint16_t Option::getLength(){
    return this->length;
}

uint8_t* Option::getValue(){
    return this->value;
}

//setters

void Option::setNumber(uint16_t num){
    this->number = num;
}

void Option::setDelta(uint16_t del){
    this->delta = del;
}

void Option::setLength(uint16_t len){
    this->length = len;
}

void Option::setValue(const char *val){

    this->value = (uint8_t*) malloc(sizeof(uint8_t) * this->length);

    //copies option value from external array into internal array
    for(uint16_t i = 0; i < this->length; i++){
        // this->value[i] = val[i];
        this->value[i] = pgm_read_byte_near(val + i);
    }
}

//parsers

int8_t Option::parseDelta(uint8_t packetBuffer[MAX_BUFFER], uint16_t firstByte){

    //reads four bites from the 'left side' of a first byte following the header
    //then sets it to this object
    this->delta = (packetBuffer[firstByte] & 0xF0) >> 4;

    //reads one byte of extended option delta and sum it with 13
    //then sets it to this object
    if(this->delta == 13){
        this->delta = 13 + packetBuffer[firstByte + 1];
        //to indicate that there is one additional byte of delta extension
        return 1;

    //read two bytes of extended option delta and sum it with 269
    //then sets it to this object
    }else if(this->delta == 14){
        this->delta = 269 + ((uint16_t)packetBuffer[firstByte + 1] << 8) + packetBuffer[firstByte + 2];
        //to indicate that there are two additional bytes of delta extension
        return 2;

    //happens when the payload marker is present
    }else if(this->delta == 15){
        this->delta = 0;
        //to indicate that the payload marker is present
        return PAYLOAD_MARKER;

    }else{
        //to indicate that there was neither a delta extension, nor the payload marker
        return 0;
    }
}

int8_t Option::parseLength(uint8_t packetBuffer[MAX_BUFFER], uint16_t firstByte){

    //reads four bites from the 'right side' of a first byte following the header
    //then sets it to this object
    this->length = packetBuffer[firstByte] & 0x0F;

    //reads one byte of extended option length and sums it with 13
    //then sets it to this object
    if(length == 13){
        this->length = 13 + packetBuffer[firstByte + 1];
        //to indicate that there is one additional byte of length extension
        return 1;

    //reads two bytes of extended option length and sums it with 269
    //then sets it to this object
    }else if(length == 14){
        this->length = 269 + ((uint16_t)packetBuffer[firstByte + 1] << 8) + packetBuffer[firstByte + 2];
        //to indicate that there are two additional bytes of length extension
        return 2;

    }else if(length == 15){
        //sets length to zero, to make sure, that value parsing loop will not execute
        this->length = 0;
        //to indicate that the payload marker is present
        return PAYLOAD_MARKER;
    }else{
        //to indicate that there was neither a length extension, nor the payload marker
        return 0;
    }
}

uint16_t Option::parseValue(uint8_t packetBuffer[MAX_BUFFER], uint16_t firstByte){
    //przy rozbudowywaniu tej funkcji pamiętaj, że value trafia później do free() i bez malloca wypierdoli błąd!!
    //allocates memory to a value array, based on option length
    this->value = (uint8_t*) malloc(sizeof(uint8_t)*this->length);

    //copies option value from a buffer into an array
    for(uint16_t i = 0; i < this->length; i++){
       this->value[i] = (packetBuffer[firstByte + i]);
    }

    //returns option length for further calculations of first payload byte
    return this->length;
}

//others

boolean Option::compareValue(const char *val){

    if(this->length == strlen_P(val)){

        for(uint16_t i = 0; i < this->length; i++){
            if(this->value[i] != pgm_read_byte_near(val + i)){
                return false;
            }
        }

        return true;
    }else{
        return false;
    }
}

//prints option details into Serial Monitor
void Option::print(){
    Serial.println(F("***Option:***"));

    Serial.print(F("   number: "));
    Serial.print(this->number); Serial.print(F("  "));

    Serial.print(F("delta: "));
    Serial.print(this->delta); Serial.print(F("  "));

    Serial.print(F("length: "));
    Serial.print(this->length); Serial.print(F("  "));

    Serial.print(F("value: "));
    for(uint8_t i = 0; i < this->length; i++){
        Serial.print(this->value[i], HEX); Serial.print(F("; "));
    }

    Serial.println(F(""));
}

//frees dynamically allocated memory
void Option::freeMemory(){
    free(this->value);
    fix_malloc_bug();
    this->value = NULL;
}
