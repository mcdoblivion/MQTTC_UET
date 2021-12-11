#include "message.h"
#include <string.h>
#include "mqtt.h"
#include "mynet.h"

static const char *mes_type_text[] = {
    "CON",
    "CONACK",
    "PUB",
    "PUBACK",
    "SUB",
    "SUBACK",
    "UNSUB",
    "UNSUBACK",
    "DISCON"};

const char *mes_type_tostring(mqtt_mes_type type)
{
    return mes_type_text[(int)type];
}

variable_header *variable_header_new(char *key, char *data)
{
    variable_header *newVarHeader = (variable_header *)malloc(sizeof(variable_header));
    newVarHeader->key = key;
    newVarHeader->data = data;
    return newVarHeader;
}

message *mes__new()
{
    message *newMes = (message *)malloc(sizeof(message));
    if (!newMes)
    {
        printf("fail when init message\n");
        return NULL;
    }
    memset(newMes, 0, sizeof(newMes));
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
void mes_set_variable_header(message *mes, char *key, char *data)
{
    uint16_t key_len = strlen(key);
    uint16_t data = strlen(data);

    variable_header *vh = NULL;
    vh = variable_header_new(key, data);
    mes->variable_header = vh;
    mes->variable_size = strlen(vh->data);
}

void mes_set_payload(message *mes, uint8_t *payload, uint32_t payload_size)
{
    if (payload_size == 0)
    {
        return;
    }
    else
    {
        mes->payload = (uint8_t *)mem__malloc(sizeof(uint8_t) * payload_size);
        mes->payload_size = payload_size;
        memcpy(mes->payload, payload, payload_size);
    }
}

void mes_CON(message *mes, uint8_t *payload_data, uint32_t payload_size)
{
    mes_set_flag(mes, 0);
    mes_set_message_type(mes, CON);
    mes_set_payload(mes, payload_size, payload_data);
}

void mes_PUB(message *mes, char *topic, uint8_t flag, uint8_t *payload, uint32_t size)
{
    // on message publish, payload is content of message which will be published to topic
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, PUB);
    mes_set_variable_header(mes, "q-topic", topic);
    mes_set_payload(mes, payload, size);
}

void mes_SUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, SUB);

    if (topic)
    {
        mes_set_variable_header(mes, "mes-id", mes_id);
        uint8_t payload_size = strlen(topic);
        mes_set_payload(mes, topic, payload_size);
    }
}

void mes_UNSUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    mes_set_flag(mes, flag);
    mes_set_message_type(mes, UNSUB);
    if (topic)
    {
        mes_set_variable_header(mes, "mes-id", mes_id);
        uint8_t payload_size = strlen(topic);
        mes_set_payload(mes, topic, payload_size);
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
        mes_set_variable_header(dst, "mes-ack", "200 OK");
    }
}

void mes_send(mqtt_connection *con, message *mes)
{
    uint8_t fixed_header[FIXED_HEADER_SIZE];
    uint16_t vars_size = mes->variable_size;
    uint16_t mes_type = mes->mes_type;
    uint16_t flag = mes->flag;
    uint16_t payload_size = mes->payload_size;
    memcpy(fixed_header + OFFSET_REMAIN_VAR_SIZE, &vars_size, sizeof(vars_size));
    memcpy(fixed_header + OFFSET_MESSAGE_TYPE, &mes_type, sizeof(mes_type));
    memcpy(fixed_header + OFFSET_FLAG, &flag, sizeof(flag));
    memcpy(fixed_header + OFFSET_REMAIN_PAYLOAD_SIZE, &payload_size, sizeof(payload_size));

    mynet_write(con, fixed_header, FIXED_HEADER_SIZE);

    if (mes->variable_size > 0)
    {
        uint8_t var_header_buff = (uint8_t *)malloc(sizeof(uint8_t) * vars_size);

        mynet_write(con, var_header_buff, mes->variable_size);
    }
    if (mes->payload_size > 0)
    {
        uint8_t payload_buf = (uint8_t *)malloc(sizeof(uint8_t) * payload_size);

        mynet_write(con, payload_buf, mes->payload_size);
    }
}

void mes_recv(mqtt_connection *con, message *mes)
{
    // make empty mes
    uint8_t fixed_header[FIXED_HEADER_SIZE];

    mynet_read(con, fixed_header, FIXED_HEADER_SIZE);

    uint16_t vars_size = mes->variable_size;
    uint16_t mes_type = mes->mes_type;
    uint16_t flag = mes->flag;
    uint16_t payload_size = mes->payload_size;
    memcpy(fixed_header + OFFSET_REMAIN_VAR_SIZE, &vars_size, sizeof(vars_size));
    memcpy(fixed_header + OFFSET_MESSAGE_TYPE, &mes_type, sizeof(mes_type));
    memcpy(fixed_header + OFFSET_FLAG, &flag, sizeof(flag));
    memcpy(fixed_header + OFFSET_REMAIN_PAYLOAD_SIZE, &payload_size, sizeof(payload_size));

    // the ideal is make "mes" new and parse data from "con" to each part in mes
    // net__read(conn, var_hdr_buf, vars_size, &n_read, 1);
    // frame__parse_var_header(frame, var_hdr_buf, vars_size);
    // frame__malloc_payload(frame, data_size);
    // net__read(conn, frame->payload, data_size, &n_read, 1);
}