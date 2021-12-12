#include "mqtt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mynet.h"
#include "message.h"

static const char *mes_type_text[] = {
    "CON",
    "CONACK",
    "PUB",
    "PUBACK",
    "SUB",
    "SUBACK",
    "UNSUB",
    "UNSUBACK",
    "DISCON"
};

const char *mes_type_tostring(mqtt_mes_type type)
{
    return mes_type_text[(int)type];
}

variable_header *variable_header_new(char* key, uint8_t *var_header_data)
{
    variable_header *newVarHeader = (variable_header *)malloc(sizeof(var_header_data));
    newVarHeader->key = key;
    newVarHeader->data = var_header_data;
    return newVarHeader;
}

message *mes_new()
{
    message *newMes = (message *)malloc(sizeof(message));
    if (!newMes)
    {
        printf("fail when init message\n");
        return NULL;
    }
    memset(newMes, 0, sizeof(message));
    return newMes;
}

void mes_free(message *mes)
{
    free(mes); // ?
}

variable_header *mes_get_var_header_data(message *mes)
{
    if (!mes)
        return NULL;

    return mes->variable_header;
}

void mes_set_message_type(message *mes, uint8_t mes_type)
{
    mes->mes_type = mes_type;
}

void mes_set_flag(message *mes, uint16_t flag)
{
    mes->flag = flag;
}
void mes_set_variable_header(message *mes, char* key, uint8_t *var_header_data)
{
    variable_header *vh = NULL;
    vh = variable_header_new(key, var_header_data);
    mes->variable_header = vh;
    mes->variable_size = strlen((char*)vh->data);
}

void mes_set_payload(message *mes, uint8_t *payload, uint32_t payload_size)
{
    if (payload_size == 0)
    {
        return;
    }
    else
    {
        mes->payload = (uint8_t *)malloc(sizeof(uint8_t) * payload_size);
        mes->payload_size = payload_size;
        memcpy(mes->payload, payload, payload_size);
    }
}

void mes_CON(message *mes, uint8_t *payload_data, uint32_t payload_size)
{
    mes_set_flag(mes, FLAG_CON);
    mes_set_message_type(mes, CON);
    // lack of set var header ????
    mes_set_payload(mes, payload_data, payload_size);
}

void mes_PUB(message *mes, char *topic, uint8_t flag, uint8_t *payload, uint32_t size)
{
    // on message publish, payload is content of message which will be published to topic
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, PUB);
    mes_set_variable_header(mes, "q-topic", NULL);
    // mes_set_variable_header(mes, "q-topic", topic);

    mes_set_payload(mes, payload, size);
}

void mes_SUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, SUB);

    if (topic)
    {
        // mes_set_variable_header(mes, "mes-id", mes_id);
        mes_set_variable_header(mes, "mes-id", NULL);
        uint8_t payload_size = strlen(topic);
        // mes_set_payload(mes, topic, payload_size);
        mes_set_payload(mes, NULL, payload_size);

    }
}

void mes_UNSUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, UNSUB);
    if (topic)
    {
        // mes_set_variable_header(mes, "mes-id", mes_id);
        mes_set_variable_header(mes, "mes-id", NULL);

        uint8_t payload_size = strlen(topic);
        // mes_set_payload(mes, topic, payload_size);
        mes_set_payload(mes, NULL, payload_size);

    }
}

void mes_ACK(message *dst, message *src, char *msg)
{
    mes_set_message_type(dst, src->mes_type);
    mes_set_flag(dst, src->flag);
    mes_set_variable_header(dst, "1", src->variable_header->data);
    mes_set_message_type(dst, ACK);

    if (!strlen(msg))
    {
        msg = "BROKER received msg";
        mes_set_payload(dst, (uint8_t *)msg, strlen(msg));
        // mes_set_variable_header(dst, "mes-ack", "200 OK");
        mes_set_variable_header(dst, "mes-ack", NULL);

    }
}

void mes_send(mqtt_connection *con, message *mes)
{
    uint8_t fixed_header[FIXED_HEADER_SIZE];
    uint16_t mes_type = mes->mes_type;
    uint16_t flag = mes->flag;
    uint16_t vars_size = mes->variable_size;
    uint16_t payload_size = mes->payload_size;

    memcpy(fixed_header + OFFSET_MESSAGE_TYPE, &mes_type, sizeof(mes_type));
    memcpy(fixed_header + OFFSET_FLAG, &flag, sizeof(flag));
    memcpy(fixed_header + OFFSET_REMAIN_VAR_SIZE, &vars_size, sizeof(vars_size));
    memcpy(fixed_header + OFFSET_REMAIN_PAYLOAD_SIZE, &payload_size, sizeof(payload_size));

    //////////////////////// need consider why do we sent each part of mes but not all mes ?
    mynet_write(con, fixed_header, FIXED_HEADER_SIZE);

    if (mes->variable_size > 0)
    {
        uint8_t *var_header_buff = (uint8_t *)malloc(sizeof(uint8_t) * vars_size);

        mynet_write(con, var_header_buff, mes->variable_size);
    }
    if (mes->payload_size > 0)
    {
        uint8_t *payload_buf = (uint8_t *)malloc(sizeof(uint8_t) * payload_size);

        mynet_write(con, payload_buf, mes->payload_size);
    }
}

void mes_recv(mqtt_connection *con, message *mes)
{
    // make empty mes
    uint8_t fixed_header[FIXED_HEADER_SIZE];
    mynet_read(con, fixed_header, FIXED_HEADER_SIZE);

    uint16_t vars_size;
    uint16_t mes_type;
    uint16_t flag;
    uint16_t payload_size;
    memcpy(&mes_type, fixed_header + OFFSET_MESSAGE_TYPE, sizeof(mes_type));
    memcpy(&flag, fixed_header + OFFSET_FLAG, sizeof(flag));
    memcpy(&vars_size, fixed_header + OFFSET_REMAIN_VAR_SIZE, sizeof(vars_size));
    memcpy(&payload_size, fixed_header + OFFSET_REMAIN_PAYLOAD_SIZE, sizeof(payload_size));
    
    // set fix header
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, mes_type);
    mes->variable_size = vars_size;
    mes->payload_size = payload_size;

    // set variable_header_data
    uint8_t* variable_header_data = (uint8_t*)malloc(sizeof(uint8_t) * vars_size);
    mynet_read(con, variable_header_data, vars_size);
    mes_set_variable_header(mes, "mes_recv" , variable_header_data);
//   set payload_data
    uint8_t* payload_data = (uint8_t*)malloc(sizeof(uint8_t) * payload_size);
    mynet_read(con, payload_data, payload_size);
    mes_set_payload(mes, payload_data, payload_size);

}