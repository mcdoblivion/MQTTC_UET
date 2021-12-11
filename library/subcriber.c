#include "mqtt.h"
#include "subcriber.h"
#include "client.h"
#include "topic.h"

char* subcriber_init_id(char* topic, client* client){
    size_t topic_name_len = strlen(topic);
    size_t client_id_len = strlen(client->id);
    char* id = malloc(sizeof(char)* (topic_name_len + client_id_len + 2 ));
    memmove(id, client->id, client_id_len);
    *(id + client_id_len) = "-";
    memmove(id + client_id_len + 1, topic, topic_name_len);
    *(id + client_id_len +  topic_name_len + 1) = "\0";

    return id;
}

char* subcriber_get_topic(subcriber* sub){
    return sub->id + strlen(sub->client->id) + 1;
}
char* subcriber_get_clientId(subcriber* sub){
    return sub->client->id;
}

subcriber* subcriber_new(char* topic, client* client, topic_node* node){
    subcriber* new_sub = (subcriber*)malloc(sizeof(subcriber));
    memset(new_sub, 0, sizeof(*new_sub));
    new_sub->id = subcriber_init_id(topic, client);
    new_sub->client = client;
    new_sub->node = node;

    return new_sub;

}

void subcriber_free(subcriber* sub){
    free(sub);
}