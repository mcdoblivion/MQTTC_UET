#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "mqtt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mynet.h"

#define FIXED_HEADER_SIZE 16 // bytes
#define OFFSET_MESSAGE_TYPE 0
#define OFFSET_FLAG 4
#define OFFSET_REMAIN_VAR_SIZE 8
#define OFFSET_REMAIN_PAYLOAD_SIZE 12

typedef enum
{
    CON = 1,
    PUB,
    SUB,
    SUBACK,
    UNSUB,
    UNSUBACK,
    ACK,
    DISCON
} mqtt_mes_type;

typedef enum
{
    FLAG_CON = 0x01,
    FLAG_PUB = 0x02,
    FLAG_SUB = 0x03,
    FLAG_SUBACK,
    flag_UNSUB,
    FLAG_UNSUBACK,
    FLAG_ACK,

} mqtt_flag;

struct variable_header
{
    char *key;
    uint8_t *data;
};

struct message
{
    // fixed header
    uint8_t mes_type;
    uint8_t flag;
    uint8_t variable_size;
    uint8_t payload_size;

    //variable header
    variable_header *variable_header;

    //payload
    uint8_t* payload;

};

message *mes_new();
const char *mes_type_tostring(mqtt_mes_type type);
variable_header *variable_header_new(char* key, uint8_t *var_header_data);
void mes_free(message *mes);

// set and get
variable_header *mes_get_var_header_data(message *mes);
void mes_set_message_type(message *mes, uint8_t mes_type);
void mes_set_flag(message *mes, uint16_t flag);
void mes_set_variable_header(message *mes, char* key, uint8_t *var_header_data);
void mes_set_payload(message *mes, uint8_t *payload, uint32_t payload_size);

//classify type of mes
void mes_CON(message *mes, uint8_t *info, uint32_t payload_size);
void mes_PUB(message *mes, char *topic, uint8_t flag, uint8_t *payload, uint32_t size);
void mes_SUB(message *mes, uint8_t flag, char *mes_id, char *topic);
void mes_UNSUB(message *mes, uint8_t flag, char *mes_id, char *topic);
void mes_ACK(message *dst, message *src, char *msg);

//action with mes
void mes_send(mqtt_connection *con, message *mes);
void mes_recv(mqtt_connection *con, message *mes);

#endif
