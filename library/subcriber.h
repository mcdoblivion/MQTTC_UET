#ifndef _SUBCRIBER_H_
#define _SUBCRIBER_H_

#include "mqtt.h"
#include "client.h"
#include "topic.h"

struct subcriber
{
    char *id;
    client *client;
    topic* topic
};


#endif