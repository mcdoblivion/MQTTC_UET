#include "topic.h"
#include "string.h"
#include "mqtt.h"
#include "subcriber.h"

topic_node *topic_node_new()
{
    topic_node *n = (topic_node *)malloc(sizeof(topic_node));
    memset(n, 0, sizeof(*n));
    return n;
}

void topic_node_add_sub(topic_node *n, subcriber *s)
{
    s->topic_node = n;
    subcriber *ptr = n->sub_list;
    ptr = &s;
    memcpy(n->sub_list, ptr, 10);
    // need to edit soon
}

void topic_init_node(topic_node *node)
{
    memset(node, 0, sizeof(*node));
}
void topic_init_tree(topic_tree *tree)
{
    memset(tree, 0, sizeof(*tree));
    topic_init_node(tree->root);
}

topic_tree *topic_new_tree()
{
    topic_tree *tree = (topic_tree *)malloc(sizeof(topic_tree));
    topic_init_tree(tree);
    return tree;
}

void topic_extract_token(char *topic, int *len, char **first_topic)
{
    char *token;
    const char s[2] = "/";
    token = strtok(topic, s);
    *first_topic = token;
    *len = 0;
    while (token != NULL)
    {
        *len++;
        //printf(" %s\n", token);
        token = strtok(NULL, s);
    }
}

void topic_add_sub(topic_tree *tree, subcriber *subcriber)
{
    char *first_topic;
    int len = 0;
    char *topic = subcriber_get_topic(subcriber);
    topic_extract_token(topic, len, first_topic);

    topic_node *cur_topic = &tree->root;
    while (len > 0)
    {
        len--;
        cur_topic = cur_topic + strlen(cur_topic) + 1;
        topic_node_add_sub(cur_topic, subcriber);
    }
    free(first_topic);
}

void topic_rmv_sub(topic_tree *tree, subcriber *subcriber)
{
    topic_node *node = subcriber->topic_node;
    char *first_topic;
    int len = 0;
    char *topic = subcriber_get_topic(subcriber);
    topic_extract_token(topic, len, first_topic);

    subcriber->topic_node = NULL;
}

char *topic_node_find_sub(topic_node *node, char *first_topic, int len)
{
    print("thisnotthing");
}

char *topic_tree_find_sub(topic_tree *tree, char *topic)
{
    char *first_topic;
    int len = 0;
    topic_extract_token(topic, len, first_topic);

    char *s = topic_node_find_sub(tree->root, first_topic, len);
    return s;
}
