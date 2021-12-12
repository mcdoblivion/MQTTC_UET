#include <pthread.h>
#include "mqtt.h"
#include "message.h"
#include "topic.h"
#include "mynet.h"
#include "broker.h"
#include "subcriber.h"
#include "client.h"


void rand_str_id(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

client* client_new(mqtt_connection* connection, broker* broker){
    client* client = malloc(sizeof(struct client));
    rand_str_id(client->id, 5);
    client->connection = connection;
    client->broker= broker;
    client->income = mes_new();
    client->outcome = mes_new();
    return client;

}

void client_free(client* client){
    free(client);
}

void client_receive(client* client, message* mes){
    uint8_t fixed_header[FIXED_HEADER_SIZE];
    size_t read_size = 0;
    mynet_read(client->connection, fixed_header, read_size);

    uint8_t mes_type;
    uint8_t variable_header_size;
    uint8_t flag;
    memcpy(&mes_type, fixed_header + OFFSET_MESSAGE_TYPE, sizeof(mes_type));
    memcpy(&variable_header_size, fixed_header+ OFFSET_REMAIN_VAR_SIZE, sizeof(variable_header_size));
    memcpy(&flag, fixed_header + OFFSET_FLAG, sizeof(flag));

    //set fixed header
    mes_set_message_type(mes, mes_type);
    mes_set_flag(mes, flag);

    //set variable header
    uint8_t* var_h_buff = (uint8_t*)malloc(sizeof(uint8_t) * variable_header_size);
    mynet_read(client->connection, var_h_buff, read_size);


}

void client_send(client* client, message* mes){
    mes_send(client->connection, mes);
}