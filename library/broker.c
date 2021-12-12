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

    // //init mutex
    pthread_mutex_init(&b->mutex, NULL);
    //init topics of broker
    topic_tree *topics = topic_new_tree();
    b->topics = topics;
    // //init listener
    mqtt_connection *listener = mynet_listen(host, port);
    b->listener = listener;
    b->isActive = true;

    return b;
}

void rmvBroker(broker *b)
{
    pthread_mutex_lock(&b->mutex);
    doCloseBroker(b, 0);
    pthread_mutex_unlock(&b->mutex);
    free(b->listener);
    free(b->topics);
    pthread_mutex_destroy(&b->mutex);
    free(b);
}

client *doBrokerAccept(broker *b)
{
    mqtt_connection *con = mynet_accept(b->listener);
    client *cli = client_new(con, b);

    ///////////////////// why do you assign a cli to a pointer list?
  //  b->clientList = cli;
    return cli;
}

void doBrokerSendMessage(client *cliSender, subcriber *subcriber, message *mes)
{
    broker *b = cliSender->broker;
    // mes_set_variable_header(mes, "topic", subcriber_get_topic(subcriber));
    mes_set_variable_header(mes, NULL, NULL,NULL);


    client *cli;
    char *clientId = subcriber->client->id;

    //need to write function to get client has id = clientid from broker->clients, after that return for clirecv
    client* cliRecv = NULL;

    if (cliRecv == subcriber->client) //this line may be redundant
    {
        char *srcIP = inet_ntoa(cliSender->connection->addr->sin_addr);
        char *srcAddr = (char *)malloc(sizeof(char) * (strlen(srcIP) + 10));
        sprintf(srcAddr, "%s:%d", srcIP, ntohs(cliSender->connection->addr->sin_port));
        // mes_set_variable_header(mes, "from:", srcAddr);
        mes_set_variable_header(mes, NULL, NULL, NULL);

        free(srcAddr); //consider rmv this line?
        client_send(cliRecv, mes);
    }
}

void doBrokerAddSubcriber(broker* b, subcriber* s){
    topic_add_sub(b->topics, s);
}

void doBrokerRmvSubcriber(broker* b, subcriber* s){
    topic_rmv_sub(b->topics, s);
}

char* doBrokerFindSubcriber(broker* b, char* topic){
    char* result = topic_tree_find_sub(b->topics, topic);
    return result;
}
