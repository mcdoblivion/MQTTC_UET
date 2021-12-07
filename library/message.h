#ifndef _MESSAGE_H_
#define _MESSAGE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "status.h"


#define FIXED_HEADER_SIZE 16  // bytes
#define OFFSET_MESSAGE_TYPE 0
#define OFFSET_FLAG 4
#define OFFSET_REMAIN_VAR_SIZE 8
#define OFFSET_REMAIN_PAYLOAD_SIZE 12

typedef enum {
    CON = 1,
    CONACK,
    PUB,
    PUBACK,
    SUB,
    SUBACK,
    UNSUB,
    UNSUBACK,
    DISCON
} mqtt_mes_type;

typedef enum {
    flag_ack = 0x80,
    flag_puback = 0x40
} mqtt_flag;

typedef struct variable_header {
    char* key;
    char* data;
} variable_header;


typedef struct message {
    // mes_type
    // flag
    // variable_header_size
    // payload_size
    // variable_header
    // payload

    // fixed header
    uint8_t mes_type;
    uint8_t flag;
    uint8_t variable_size;
    uint8_t payload_size;

    //variable header
    variable_header* variable_header;
    
    //payload
    uint8_t payload;

} message;

// status mes__free()
message* mes_new();
// void mes_clear
// void mes_get_var_header
// void mes_set_var_header
// void mes_set_flag
// void

// status mes_recv(mqtt_connection* conn, message* mes);
// status mes_send(mqtt_connection* conn, message* mes);

status mes_CON(message *mes, uint8_t *info, uint32_t payload_size);
status mes_PUB(message *mes, char *topic, uint8_t flag, uint8_t *payload, uint32_t size);
status mes_SUB(message *mes, uint8_t flag, char *mes_id, char *topic);
status mes_UNSUB(message *mes, uint8_t flag, char *mes_id, char *topic);

#endif
