#ifndef _TOPIC_H_
#define _TOPIC_H_

#include "string.h"
#include "mqtt.h"
#include "subcriber.h"

#define MAX_SUBCRIBER_LEN 10

typedef struct topic_node topic_node;
typedef struct topic_tree topic_tree;

struct topic_node
{
    char* id;
    topic_node *child_node;
    topic_node *parent_node;
    subcriber* sub_list;
};

struct topic_tree
{
    topic_node *root;
    pthread_mutex_t mutex;
};

topic_tree *topic_new_tree();
void topic_extract_token(char *topic, int *len, char **first_topic);
void topic_add_sub(topic_tree *tree, subcriber *subcriber);
void topic_rmv_sub(topic_tree *tree, subcriber *subcriber);
char *topic_node_find_sub(topic_node *node, char *first_topic, int len);
char *topic_tree_find_sub(topic_tree *tree, char *topic);

#endif // _TOPIC_H_
