#include "message.h"
#include <string.h>
#include "message.h"
#include "status.h"

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

status mes_set_message_type(message *mes, uint8_t mes_type)
{
    status s = OK;

    mes->mes_type = mes_type;

    return s;
}

status mes_set_flag(message *mes, uint16_t flag)
{
    status s = OK;

    mes->flag = flag;

    return s;
}
status mes_set_variable_header(message *mes, char *key, char *data)
{
    status s = OK;
    uint16_t key_len = strlen(key);
    uint16_t data = strlen(data);

    variable_header *vh = NULL;
    vh = variable_header_new(key, data);
    mes->variable_header = vh;
    mes->variable_size = strlen(vh->data);

    return s;
}

status mes_set_payload(message *mes, uint8_t *payload, uint32_t payload_size)
{
    status s = OK;
    if (payload_size == 0)
    {
        s = ERROR;
    }
    else
    {
        mes->payload = (uint8_t *)mem__malloc(sizeof(uint8_t) * payload_size);
        mes->payload_size = payload_size;
        memcpy(mes->payload, payload, payload_size);
    }

    return s;
}

status mes_CON(message *mes, uint8_t *info, uint32_t payload_size)
{
    status s = mes_set_flag(mes, 0);
    s = mes_set_message_type(mes, CON);
    s = mes_set_payload(mes, payload_size, info);
    return s;
}

status mes_PUB(message *mes, char *topic, uint8_t flag, uint8_t *payload, uint32_t size)
{
    // on message publish, payload is content of message which will be published to topic
    status s = mes_set_flag(mes, flag);
    s = mes_set_message_type(mes, PUB);
    s = mes_set_variable_header(mes, "q-topic", topic);
    s = mes_set_payload(mes, payload, size);
    return s;
}

status mes_SUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    status s = mes_set_flag(mes, flag);
    s = mes_set_message_type(mes, SUB);

    if (topic)
    {
        s = mes_set_variable_header(mes, "mes-id", mes_id);
        uint8_t payload_size = strlen(topic);
        s = mes_set_payload(mes, topic, payload_size);
    }
}

status mes_UNSUB(message *mes, uint8_t flag, char *mes_id, char *topic)
{
    status s = mes_set_flag(mes, flag);
    s = mes_set_message_type(mes, UNSUB);
    if (topic)
    {
        s = mes_set_variable_header(mes, "mes-id", mes_id);
        uint8_t payload_size = strlen(topic);
        s = mes_set_payload(mes, topic, payload_size);
    }
}


