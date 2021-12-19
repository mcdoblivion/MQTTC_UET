#include <pthread.h>
#include "mqtt.h"
#include "message.h"
#include "topic.h"
#include "mynet.h"
#include "broker.h"
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