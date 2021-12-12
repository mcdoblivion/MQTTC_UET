#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>
#include "mqtt.h"
#include "message.h"
#include "topic.h"
#include "mynet.h"
#include "broker.h"
#include "subcriber.h"

// typedef struct client client;

struct client {
    char id[5];
    broker* broker;
    mqtt_connection* connection;
    subcriber* subcribers;
    message* income;
    message* outcome;

}; 

client* client_new(mqtt_connection* connection, broker* broker);
void client_send(client* client, message* mes);
void client_receive(client* client, message* mes);


#endif