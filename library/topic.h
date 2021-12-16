#ifndef _TOPIC_H_
#define _TOPIC_H_

#include "string.h"
#include "mqtt.h"
#include "subcriber.h"

#define MAX_SUBCRIBER_LEN 10

//struct
struct topic
{
    char* name;
    client* clients[MAX_SUBCRIBER_LEN];
    topic* next;
};

//function
void topic_extract_token(char *topic, int len, char *first_topic);
topic* topic_find_sub(topic *head, char *topic_name);
int topic_get_clients_length(topic* t);


#endif // _TOPIC_H_
