#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>
#include "mqtt.h"
#include "message.h"
#include "topic.h"
#include "mynet.h"
#include "broker.h"

struct  client {
    char id[5];
    broker* broker;
    mqtt_connection* connection;
    message* income;
    message* outcome;
}; 

//function
client* client_new(mqtt_connection* connection, broker* broker);
void client_free(client* client);

#endif