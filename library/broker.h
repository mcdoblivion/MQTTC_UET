#ifndef _BROKER_H_
#define _BROKER_H_

#include "mqtt.h"
#include "client.h"
#include "topic.h"
#include "mynet.h"
#include "pthread.h"
#include <stdbool.h>
#include "subcriber.h"

#define MAX_TOPIC_LIST_LENGTH 100
// model
struct broker
{
    mqtt_connection *listener;
    pthread_mutex_t mutex;
    topic_tree *topic_tree;
    topic* topic_head;
    client *clientList;
    bool isActive;
};

// funtion
void doCloseBroker(broker *broker, uint8_t lock);
broker *initBroker(char *host, uint16_t port);
void rmvBroker(broker *b);
client *doBrokerAccept(broker *b);
// void doBrokerPulishMessage(client *cliSrc, client* cliRcvList, message *mes);
void doBrokerPulishMessage(client *cliSrc, topic* t, char* data);
void doBrokerAddSubcriber(broker* b, char* topic_name, client* client);
// void doBrokerRmvSubcriber(broker *b, subcriber *sub);
void doBrokerRmvSubcriber(broker *b, char *topic_name, client *cli);
client *doBrokerFindSubcriber(broker *b, char *topic);
topic *doBrokerFindTopicNode(broker *b, char *topic);

#endif // _BROKER_H_
