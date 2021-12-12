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

//reduntant code
static const char *MESSAGE_TYPE_TEXT[] = {
    "CON",
    "CON_ACK",
    "PUB",
    "PUB_ACK",
    "SUB",
    "SUB_ACK",
    "UN_SUB",
    "UN_SUB_ACK",
    "DISCON"};
//reduntant code
const char *MESSAGE_TYPE_TO_STRING(MQTT_MESSAGE_TYPE type)
{
    return MESSAGE_TYPE_TEXT[(int)type];
}

message *mes_new()
{
    message *new_message = (message *)malloc(sizeof(struct message));
    if (!new_message)
    {
        printf("fail when init message\n");
        return NULL;
    }
    memset(new_message, 0, sizeof(message));
    return new_message;
}

void mes_empty(message *mes)
{
    if (mes->payload)
    {
        free(mes->payload);
    }
    mes->payload = NULL;
    mes->payload_size = 0;
    if (mes->variable_header)
    {
        free(mes->variable_header);
    }
}

void mes_free(message *mes)
{
    free(mes);
}

void mes_set_message_type(message *mes, uint8_t mes_type)
{
    mes->mes_type = mes_type;
}

void mes_set_flag(message *mes, uint8_t flag)
{
    mes->flag = flag;
}

void mes_set_variable_hdr(message *mes, uint8_t *variable_hdr, uint8_t variable_hdr_size)
{
    if (variable_hdr_size == 0)
    {
        return;
    }
    else
    {
        mes->variable_header = (uint8_t *)malloc(sizeof(uint8_t) * variable_hdr_size);
        memcpy(mes->variable_header, variable_hdr, variable_hdr_size);
    }
}

void mes_set_payload(message *mes, uint8_t *payload, uint8_t payload_size)
{
    if (payload_size == 0)
    {
        return;
    }
    else
    {
        mes->payload = (uint8_t *)malloc(sizeof(uint8_t) * payload_size);
        memcpy(mes->payload, payload, payload_size);
    }
}

void mes_CON(message *mes, uint8_t *payload_data, uint8_t payload_size)
{
    mes_empty(mes);
    mes_set_flag(mes, FLAG_CON);
    mes_set_message_type(mes, CON);
    mes->payload_size = payload_size;
    mes->variable_size = 0;
    mes_set_variable_hdr(mes, NULL, 0);
    mes_set_payload(mes, payload_data, payload_size);
}

void mes_DISCON(message *mes, uint8_t *variable_hdr_data, uint8_t variable_hdr_size)
{
    mes_empty(mes);
    mes_set_flag(mes, DISCON);
    mes_set_message_type(mes, DISCON);
    mes->payload_size = 0;
    mes->variable_size = variable_hdr_size;
    mes_set_variable_hdr(mes, variable_hdr_data, variable_hdr_size);
    mes_set_payload(mes, NULL, 0);
}

void mes_PUB(message *mes, char *topic, uint8_t flag, uint8_t *payload, uint8_t payload_size)
{
    mes_empty(mes);
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, PUB);
    mes->payload_size = payload_size;
    mes->variable_size = strlen(topic);
    mes_set_variable_hdr(mes, topic, strlen(topic));
    mes_set_payload(mes, payload, payload_size);
}

void mes_SUB(message *mes, uint8_t flag, uint8_t *payload, uint8_t payload_size)
{
    mes_empty(mes);
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, SUB);
    mes->payload_size = payload_size;
    mes->variable_size = 0;
    mes_set_variable_hdr(mes, NULL, 0);
    mes_set_payload(mes, payload, payload_size);
}

void mes_UN_SUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, UN_SUB);
    if (topic)
    {
        // mes_set_variable_header(mes, "mes-id", mes_id);
        //   mes_set_variable_header(mes, NULL, NULL, NULL);

        uint8_t payload_size = strlen(topic);
        // mes_set_payload(mes, topic, payload_size);
        mes_set_payload(mes, NULL, payload_size);
    }
}

void mes_COPY(message *dst, message *src)
{
    mes_empty(dst);
    dst->flag = src->flag;
    dst->mes_type = src->mes_type;
    dst->variable_size = src->variable_size;

    mes_set_variable_hdr(dst, src->variable_header, src->variable_size);
    mes_set_payload(dst, src->payload, src->payload_size);
}

void mes_ACK(message *dst, message *src, char *msg)
{
    mes_COPY(dst, src);
    mes_set_flag(dst, src->flag);
    mes_set_message_type(dst, ACK);

    if (!strlen(msg))
    {
        msg = "THIS IS ACK FROM BROKER";
    }
    dst->payload_size = strlen(msg);
    mes_set_payload(dst, (uint8_t *)msg, strlen(msg));
}

void mes_send(mqtt_connection *con, message *mes)
{
    uint8_t fixed_header[FIXED_HEADER_SIZE];
    uint8_t mes_type = mes->mes_type;
    uint8_t flag = mes->flag;
    uint8_t vars_size = mes->variable_size;
    uint8_t payload_size = mes->payload_size;

    memcpy(fixed_header + OFFSET_MESSAGE_TYPE, &mes_type, sizeof(mes_type));
    memcpy(fixed_header + OFFSET_FLAG, &flag, sizeof(flag));
    memcpy(fixed_header + OFFSET_REMAIN_VAR_SIZE, &vars_size, sizeof(vars_size));
    memcpy(fixed_header + OFFSET_REMAIN_PAYLOAD_SIZE, &payload_size, sizeof(payload_size));

    //write fixed header
    mynet_write(con, fixed_header, FIXED_HEADER_SIZE);

    //write variable header
    if (mes->variable_size > 0)
    {
        uint8_t *var_header_buff = (uint8_t *)malloc(sizeof(uint8_t) * vars_size);
        memcpy(var_header_buff, mes->variable_header, mes->variable_size);

        mynet_write(con, var_header_buff, mes->variable_size);
    }

    //write payload
    if (mes->payload_size > 0)
    {
        uint8_t *payload_buf = (uint8_t *)malloc(sizeof(uint8_t) * payload_size);
        memcpy(payload_buf, mes->payload, mes->payload_size);
        mynet_write(con, mes->payload, payload_size);
    }
}

void mes_recv(mqtt_connection *con, message *mes)
{
    // make empty mes
    uint8_t fixed_header[FIXED_HEADER_SIZE];
    mynet_read(con, fixed_header, FIXED_HEADER_SIZE);

    uint8_t vars_size;
    uint8_t mes_type;
    uint8_t flag;
    uint8_t payload_size;

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
    mes->variable_header = (uint8_t *)malloc(sizeof(uint8_t) * vars_size);
    mynet_read(con, mes->variable_header, vars_size);

    //   set payload_data
    mes->payload = (uint8_t *)malloc(sizeof(uint8_t) * payload_size);
    mynet_read(con, mes->payload, payload_size);
}