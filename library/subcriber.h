#ifndef _SUBCRIBER_H_
#define _SUBCRIBER_H_

#include "mqtt.h"
#include "client.h"
#include "topic.h"

struct subcriber
{
    char *id;
    topic_node *node;
    client *client;
};

char *subcriber_init_id(char *topic, client *client);
char *subcriber_get_topic(subcriber *sub);
char *subcriber_get_clientId(subcriber *sub);
subcriber *subcriber_new(char *topic, client *client, topic_node *node);
void subcriber_free(subcriber *sub);

#endif