#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>
#include "mqtt.h"
#include "message.h"
#include "topic.h"
#include "mynet.h"
#include "broker.h"
#include "subcriber.h"

struct client {
    char id[5];

    broker* broker;
    mqtt_connection* connection;
    subcriber* subcribers;
    
    message* income;
    message* outcome;

}; 

void client_new(mqtt_connection* connection, broker* broker);
void client_send(client* client, message* mes);
void client_receive(client* client, message* mes);


#endif