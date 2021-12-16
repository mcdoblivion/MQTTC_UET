#include "mqtt.h"
#include "client.h"
#include "mynet.h"
#include "pthread.h"
#include "broker.h"
#include "subcriber.h"
#include "topic.h"

void doCloseBroker(broker *broker, uint8_t lock)
{
    if (lock)
        pthread_mutex_lock(&broker->mutex);

    mynet_close(broker->listener);
    broker->isActive = true;

    if (lock)
        pthread_mutex_unlock(&broker->mutex);
}

broker *initBroker(char *host, uint16_t port)
{
    // construct broker b
    broker *b = (broker *)malloc(sizeof(broker));
    if (!b)
        return NULL;
    else
        memset(b, 0, sizeof(*b));

    //init mutex
    pthread_mutex_init(&b->mutex, NULL);
    // //init listener
    mqtt_connection *listener = mynet_listen(host, port);
    b->listener = listener;
    b->isActive = true;
    // init TOPIC
    b->topic_head = NULL;
    return b;
}

void rmvBroker(broker *b)
{
    pthread_mutex_lock(&b->mutex);
    doCloseBroker(b, 0);
    pthread_mutex_unlock(&b->mutex);
    free(b->listener);
    pthread_mutex_destroy(&b->mutex);
    free(b);
}

client *doBrokerAccept(broker *b)
{
    mqtt_connection *con = mynet_accept(b->listener);
    if (con->status == CONNECTED)
    {
        client *cli = client_new(con, b);
        if (cli->id)
            printf("Initialized connect with \'%s\', clientID=\'%s\'\n", inet_ntoa(cli->connection->addr->sin_addr), cli->id);
        return cli;
    }
    else
        return NULL;
}

void doBrokerPulishMessage(client *cliSrc, topic *t, char *data)
{
    broker *b = cliSrc->broker;
    int cliSubLen = topic_get_clients_length(t); // length of client subcriber topic t
    message *mes = mes_new();
    printf("->>publishing to %d client\n", cliSubLen);
    mes_PUB(mes, t->name, FLAG_PUB, data, strlen(data));

    for (int i = 0; i < cliSubLen; i++)
    {
        client *cliRecv = t->clients[i];
        printf("->>sent to client_id: \'%s\', data:\'%s\'\n, topic: \'%s\'\n", cliRecv->id, data, t->name);
        mes_send(cliRecv->connection, mes);
    }
}

void doBrokerAddSubcriber(broker *b, char *topic_name, client *cli)
{
    topic *curNode = b->topic_head;
    //head node is null?
    if (curNode == NULL)
    {
        appendNode(&(b->topic_head), topic_name, cli);
        printf("+info: topic list is empty, create new topic head is \'%s\'\n", topic_name);
        printf(">>Added new client with ID \'%s\' to topic \"%s\"\n", cli->id, topic_name);

        return;
    }
    else
    { //head node contain topic which client subcribe to
        if (strcmp(curNode->name, topic_name) == 0)
        {
            //do add client to topic node
            int cli_list_len = topic_get_clients_length(curNode);
            if (cli_list_len != MAX_SUBCRIBER_LEN)
            {
                int isDuplicateClientId = 0;
                for (int i = 0; i < cli_list_len; i++)
                {
                    if (strcmp(curNode->clients[i]->id, cli->id) == 0)
                    {
                        isDuplicateClientId = 1;
                        break;
                    }
                }
                if (isDuplicateClientId == 0)
                {
                    curNode->clients[cli_list_len] = cli;
                    printf("+info: Added new client with ID \'%s\' to topic \"%s\"\n", cli->id, topic_name);
                }
                else
                {
                    printf("+error: Duplicated clientID\n");
                }
            }
            else
            {
                printf("+error: cannot subcriber more client on topic \'%s\'\n", topic_name);
            }

            return;
        }
        // topic is contained by other node
        while (curNode->next != NULL)
        {
            if (strcmp(curNode->name, topic_name) == 0)
            {
                //do add client to topic node
                int cli_list_len = topic_get_clients_length(curNode);
                if (cli_list_len != MAX_SUBCRIBER_LEN)
                {
                    curNode->clients[cli_list_len] = cli;
                    printf("+info: Added new client with ID \'%s\' to topic \"%s\"\n", cli->id, topic_name);
                    return;
                }
                else
                {
                    printf("+error: cannot subcriber more client on topic \'%s\'\n", topic_name);
                }

                return;
            }
            else
                curNode = curNode->next;
        }
        // add new node if no node contain this topic
        appendNode(&(b->topic_head), topic_name, cli);
    }
}

void doBrokerRmvSubcriber(broker *b, char *topic_name, client *cli)
{
    topic *curNode = b->topic_head;
    //head node is null?
    if (curNode == NULL)
    {
        printf("+error: there are no topic is \'%s\', cannot handle UNSubcribe\n", topic_name);
        return;
    }
    else
    { //head node contain topic which client subcribe to
        if (strcmp(curNode->name, topic_name) == 0)
        {
            //do add client to topic node
            int cli_list_len = topic_get_clients_length(curNode);
            if (cli_list_len == 0)
            {
                printf("+error: there are no have client_id \'%s\' in topic \'%s\'\n", cli->id, topic_name);
            }
            else
            {
                for (int i = 0; i < cli_list_len; i++)
                {
                    //found out client and remove it
                    if (strcmp(curNode->clients[i]->id, cli->id) == 0)
                    {
                        curNode->clients[i] = curNode->clients[cli_list_len - 1];
                        curNode->clients[cli_list_len - 1] = NULL;
                        break;
                    }
                }

                int new_cli_list_len = topic_get_clients_length(curNode);
                printf(">>sum of subcriber remaining on topic \'%s\'= %d\n", topic_name, new_cli_list_len);
                if (new_cli_list_len == 0)
                {
                    deleteNode(&(b->topic_head), topic_name);
                    printf("+info: topic %s has been deleted(have no subcriber)\n", topic_name);
                }
            }
        }
    }
}
void appendNode(struct topic **head_ref, char *new_data, client *cli)
{
    struct topic *new_node = (struct topic *)malloc(sizeof(struct topic));
    struct topic *last = *head_ref;
    client *newClients[MAX_SUBCRIBER_LEN] = {NULL};

    new_node->name = new_data;
    new_node->clients[0] = cli; ////////// need consider
    new_node->next = NULL;
    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }
    while (last->next != NULL)
        last = last->next;

    last->next = new_node;
    return;
}

void deleteNode(struct topic **head_ref, char *key)
{
    // Store head node
    struct topic *temp = *head_ref, *prev;

    // If head node itself holds the key to be deleted
    if (temp != NULL && strcmp(temp->name, key) == 0)
    {
        *head_ref = temp->next; // Changed head
        free(temp);             // free old head
        return;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->name != key)
    {
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in linked list
    if (temp == NULL)
        return;

    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp); // Free memory
}

//NOT use
client *doBrokerFindSubcriber(broker *b, char *topic)
{
    // char* result = topic_tree_find_sub(b->topic_tree, topic);
    client *result = NULL;
    return result;
}

topic *doBrokerFindTopicNode(broker *b, char *topic_name)
{
    topic *result = topic_find_sub(b->topic_head, topic_name);
    return result;
}
