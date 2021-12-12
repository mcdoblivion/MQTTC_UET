#ifndef _BROKER_H_
#define _BROKER_H_

#include "mqtt.h"
#include "client.h"
#include "topic.h"
#include "mynet.h"
#include "pthread.h"
#include <stdbool.h>
#include "subcriber.h"

// model
struct broker
{
    mqtt_connection *listener;
    pthread_mutex_t mutex;
    topic_tree *topics;
    client *clientList;
    bool isActive;
};

// funtion
void doCloseBroker(broker *broker, uint8_t lock);
broker *initBroker(char *host, uint16_t port);
void rmvBroker(broker *b);
client *doBrokerAccept(broker *b);
void doBrokerSendMessage(client *cliSender, subcriber *subcriber, message *mes);
void doBrokerAddSubcriber(broker *b, subcriber *s);
void doBrokerRmvSubcriber(broker *b, subcriber *sub);
char *doBrokerFindSubcriber(broker *b, char *topic);

#endif // _BROKER_H_
