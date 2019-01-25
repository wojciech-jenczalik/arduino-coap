#ifndef STRINGS_H
#define STRINGS_H

const char string_1[] PROGMEM  = "12345";
const char payload_1[] PROGMEM = "Option Block2 is not supported on this server";

const char bad_version[] PROGMEM   = "Version not supported";
const char bad_header[] PROGMEM    = "Wrong format of coap header";
const char bad_token[] PROGMEM     = "Incorrect token";
const char unknown_error[] PROGMEM = "An unknown error has occured";
const char bad_option[] PROGMEM    = "Wrong format of coap option";

const char unsupported_content_format[] PROGMEM    = "Unsupported content format. Try plain text or JSON encoding";
const char not_found[] PROGMEM                     = "Following Uri-Path not found";

const char if_match_not_implemented[] PROGMEM      = "Option If-Match is not supported on this server";
const char uri_host_not_implemented[] PROGMEM      = "Option Uri-Host is not supported on this server";
const char if_none_match_not_implemented[] PROGMEM = "Option If-None-Match is not supported on this server";
const char uri_port_not_implemented[] PROGMEM      = "Option Uri-Port is not supported on this server";
const char uri_query_not_implemented[] PROGMEM     = "Option Uri-Query is not supported on this server";
const char block2_not_implemented[] PROGMEM        = "Option Block2 is not supported on this server";
const char proxy_uri_not_supported[] PROGMEM       = "Proxying is not supported on this server";
const char proxy_scheme_not_supported[] PROGMEM    = "Proxying is not supported on this server";
const char block1_not_implemented[] PROGMEM        = "Option Block1 is not supported on this server";

const char speaker[] PROGMEM       = "speaker";
const char potentiometer[] PROGMEM = "potentiometer";
const char metrics[] PROGMEM       = "metrics";
const char well_known[] PROGMEM    = ".well-known";
const char core[] PROGMEM          = "core";
const char discover[] PROGMEM      = "</speaker>,\n</potentiometer>,\n</metrics>";

#endif //SRINGS_H
