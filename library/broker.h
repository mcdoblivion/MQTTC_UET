#ifndef _BROKER_H_
#define _BROKER_H_

#include "mqtt.h"
#include "client.h"
#include "topic.h"
#include "mynet.h"
#include "pthread.h"
#include <stdbool.h>

#define MAX_TOPIC_LIST_LENGTH 100
// model
struct broker
{
    mqtt_connection *listener;
    pthread_mutex_t mutex;
    topic* topic_head;
    client *clientList;
    bool isActive;
};

// funtion
broker *initBroker(char *host, uint16_t port);
void doCloseBroker(broker *broker, uint8_t lock);
void rmvBroker(broker *b);
client *doBrokerAccept(broker *b);
void doBrokerPulishMessage(client *cliSrc, topic* t, char* data);
void doBrokerAddSubcriber(broker* b, char* topic_name, client* client);
void doBrokerRmvSubcriber(broker *b, char *topic_name, client *cli);
topic *doBrokerFindTopicNode(broker *b, char *topic);

#endif // _BROKER_H_
