// enums.h
#ifndef ENUMS_H
#define ENUMS_H

#include <Arduino.h>

namespace Enums {

  enum type{
        CON = 0,
        NON = 1,
        ACK = 2,
        RST = 3
  };

  enum requestCode{
      EMPTY_MESSAGE = 0,
      GET = 1,
      POST = 2,
      PUT = 3,
      DELETE = 4
  };

  enum responseCode{
      CREATED = 201,
      DELETED = 202,
      VALID = 203,
      CHANGED = 204,
      CONTENT = 205
  };

  enum clientError{
      BAD_REQUEST = 400,
      UNAUTHORIZED = 401,
      BAD_OPTION = 402,
      FORBIDDEN = 403,
      NOT_FOUND = 404,
      METHOD_NOT_ALLOWED = 405,
      NOT_ACCEPTABLE = 406,
      PRECONDITION_FAILED = 412,
      REQUEST_ENTITY_TOO_LARGE = 413,
      UNSUPPORTED_CONTENT_FORMAT = 415
  };

  enum serverError{
      INTERNAL_SERVER_ERROR = 500,
      NOT_IMPLEMENTED = 501,
      BAD_GATEWAY = 502,
      SERVICE_UNAVAILABLE = 503,
      GATEWAY_TIMEOUT = 504,
      PROXYING_NOT_SUPPORTED = 505
  };
  //##### /dd #####

  enum formatScenarios{
      UNSUPPORTED_VERSION = -1
  };

  enum optionUtils{
      PAYLOAD_MARKER = -1
  };

  enum option{
      IF_MATCH = 1,
      URI_HOST = 3,
      ETAG = 4,
      IF_NONE_MATCH = 5,
      OBSERVE = 6,
      URI_PORT = 7,
      LOCATION_PATH = 8,
      URI_PATH = 11,
      CONTENT_FORMAT = 12,
      MAX_AGE = 14,
      URI_QUERY = 15,
      ACCEPT = 17,
      LOCATION_QUERY = 20,
      BLOCK2 = 23,
      BLOCK1 = 27,
      PROXY_URI = 35,
      PROXY_SCHEME = 39,
      SIZE1 = 60
  };

  enum contentFormat{
      JSON = 50,
      PLAIN = 0
  };
}

#endif //ENUMS_H
