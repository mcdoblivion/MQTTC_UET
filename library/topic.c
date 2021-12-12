#include "mqtt.h"
#include "topic.h"
#include "string.h"
#include "subcriber.h"

void array_extract_token(char *topic, char **first, int *len)
{
    *len = 0;
    char *p = strtok(topic, "/");
    *first = p;

    while (p != NULL)
    {
        *len++;
        p = strtok(NULL, "/");
    }
}

topic_node *topic_node_new()
{
    topic_node *n = (topic_node *)malloc(sizeof(topic_node));
    memset(n, 0, sizeof(*n));
    return n;
}

void topic_node_add_sub(topic_node *n, subcriber *s)
{
    s->node = n;
    subcriber *ptr = n->sub_list;
    ptr = s;
    memcpy(n->sub_list, ptr, 10);
    // need to edit soon
}

topic_node *topic_init_node(topic_node *node)
{
    topic_node *newNode = (topic_node *)malloc(sizeof(topic_node));
    memset(newNode, 0, sizeof(topic_node));
    return newNode;
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

void topic_add_sub(topic_tree *tree, subcriber *subcriber)
{
    char *first_topic = NULL;
    int len = 0;
    char *topic = subcriber_get_topic(subcriber);
    array_extract_token(topic, &first_topic, &len);

    topic_node *cur_topic = tree->root; ///////////////// need to check
    while (len > 0)
    {
        len--;
        // cur_topic = cur_topic + strlen(cur_topic) + 1;
        topic_node_add_sub(cur_topic, subcriber);
    }
    free(first_topic);
}

void topic_rmv_sub(topic_tree *tree, subcriber *subcriber)
{
    topic_node *node = subcriber->node;
    char *first_topic = NULL;
    int len = 0;
    char *topic = subcriber_get_topic(subcriber);
    array_extract_token(topic, &first_topic, &len);

    subcriber->node = NULL;
}

char *topic_node_find_sub(topic_node *node, char *first_topic_field, int len)
{
    printf("topic_node_find_sub\n");
}

// char *topic_node_find_sub(topic_node *node)
// {
//     subcriber* subs = node->sub_list;
//     int subs_len = sizeof(subs)/sizeof(subcriber);
//     char* head = NULL;
//     char* clientID_list = head;
//     for(int i=0; i < len; i++){
//         head
//     }

// }

char *topic_tree_find_sub(topic_tree *tree, char *topic)
{
    char *first_topic_field = NULL;
    int len = 0;
    array_extract_token(topic, &first_topic_field, &len);

    char *subcribers = topic_node_find_sub(tree->root, first_topic_field, len);
    free(first_topic_field);
    return subcribers;
}

topic* topic_find_sub(topic *head, char *topic_name)
{
    if (head == NULL)
    {
        printf("+info: List topic is null\n");
        return NULL;
    }
    else
    {
        struct topic *current = head; // Initialize current
        while (current != NULL)
        {
            if (strcmp(current->name, topic_name) == 0)
            {
                printf("+info: had found client_list\n");
                return current;
            }
            current = current->next;
        }
        //if had not found any client for this topic_name, create new topic with list of subcriber is null

        return NULL;
    }
}

int topic_get_clients_length(topic *t)
{
    int count = 0;
    for (int i = 0; i < MAX_SUBCRIBER_LEN; i++)
    {
        if (t->clients[i] == NULL)
        {
            break;
        }
        else
            count++;
    }
    return count;
}
