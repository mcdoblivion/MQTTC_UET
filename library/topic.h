#ifndef _TOPIC_H_
#define _TOPIC_H_

#include "string.h"
#include "mqtt.h"
#include "subcriber.h"

#define MAX_SUBCRIBER_LEN 10

typedef struct topic_node topic_node;
typedef struct topic_tree topic_tree;

// now using topic
struct topic
{
    char* name;
    client* clients[MAX_SUBCRIBER_LEN];
    topic* next;
};

//consider not using this topic_node & topic_tree
struct topic_node
{
    char* id;
    topic_node **next_node;
    topic_node *prev_node;
    subcriber* sub_list;
};

struct topic_tree
{
    topic_node *root;
    pthread_mutex_t mutex;
};

topic_tree *topic_new_tree();
void topic_extract_token(char *topic, int len, char *first_topic);
void topic_add_sub(topic_tree *tree, subcriber *subcriber);
void topic_rmv_sub(topic_tree *tree, subcriber *subcriber);
char *topic_node_find_sub(topic_node *node, char *first_topic, int len);
char *topic_tree_find_sub(topic_tree *tree, char *topic);

//using this
// client* topic_find_sub(topic* head, char* topic_name);
topic* topic_find_sub(topic *head, char *topic_name);
int topic_get_clients_length(topic* t);


#endif // _TOPIC_H_
