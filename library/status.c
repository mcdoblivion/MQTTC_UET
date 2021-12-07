#include "status.h"

static const char* status_str[] = {
    "Ok",
    "Error",
    "INVALID_TYPE_MESSAGE",
    "INVALID_TOPIC",
    "INVALID_FLAG",
    "NO_RESPONSE",
};

const char* status_to_string(status s){
    return status_str[(int) s];
}