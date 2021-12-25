#include "mqtt.h"
#include "topic.h"
#include "string.h"

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

topic* topic_find_subcribers(topic *head, char *topic_name)
{
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        struct topic *current = head; // Initialize current
        while (current != NULL)
        {
            if (strcmp(current->name, topic_name) == 0)
            {
                printf("+info: had found client_list for topic \'%s\'\n", topic_name);
                return current;
            }
            current = current->next;
        }

        //return null if not having any client subscribe
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
