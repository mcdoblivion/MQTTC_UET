#ifndef _STATUS_H_
#define _STATUS_H_

typedef enum {
    OK = 0, 
    ERROR,
    INVALID_TYPE_MESSAGE,
    INVALID_TOPIC,
    INVALID_FLAG,
    NO_RESPONSE,

} status;

const char* status_to_string(status s);

#endif
