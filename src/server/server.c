/**
 * @file server.c
 * 
 * Server program
 * Nguyen Minh Thang
 * Nguyen Hong Thai
 * Tran Nguyen Phuong Nam
 * Dong Minh Cuong
 */
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "mqtt.h"
#include "broker.h"
#include "message.h"
#include "mynet.h"

#define DEFAULT_PORT 4444
#define DEFAULT_ADDR "127.0.0.1"

broker *my_broker = NULL;

void serverDoSendAck(client *cli, message *mesIn, char *msg)
{
    message *mesOut = cli->outcome;
    mes_ACK(mesOut, mesIn, msg);
    mes_send(cli->connection, mesOut);
}

void serverHandleConnection(client *cli)
{
    char *msg = "200 HELLO CLIENT";
    serverDoSendAck(cli, cli->income, msg);
}

void serverHandlePublisher(client *cli)
{
    // printf("+status: handling publisher...\n");
    message *client_PUB_message = cli->income;

    //find topic from mes income
    char *topic_name = client_PUB_message->variable_header;
    char *data = client_PUB_message->payload;
    printf(">>New message received: \'%s\', topic: \'%s\', from client: \'%s\'.\n", data, topic_name, cli->id);

    if (topic_name && data)
    {
        char *msg = "PUBLISH OK";
        serverDoSendAck(cli, cli->income, msg);
    }

    //broker find topic have name is topic_name above
    topic *t = doBrokerFindTopicNode(cli->broker, topic_name);
    int cliSubLen = 0;
    if (t != NULL)
    {
        cliSubLen = topic_get_clients_length(t);
    }

    if (cliSubLen > 0)
    {
        // printf("+info: publishing to each client in list\n");
        doBrokerPulishMessage(cli, t, data);
    }
    else
    {
        printf("+info: have no client for this topic.\n");
    }
}

void serverHandleSubscriber(client *cli)
{
    message *client_SUB_message = cli->income;

    //find topic from mes income
    char *topic = client_SUB_message->payload;
    char *msg;
    if (topic != NULL)
    {
        doBrokerAddSubcriber(cli->broker, topic, cli);
        msg = "SUBCRIBER OK";
    }
    else
    {
        printf("invalid topic, cannot handle subcribe\n");
        msg = "SUBCRIBER FAIL";
    }

    serverDoSendAck(cli, cli->income, msg);
}

void handleUnsubscribe(client *cli)
{
    // printf("+status: handling unsubcribe...\n");
    message *client_SUB_message = cli->income;

    //find topic from mes income
    char *topic = client_SUB_message->payload;
    char *msg;
    if (topic != NULL)
    {
        doBrokerRmvSubcriber(cli->broker, topic, cli);
        msg = "UNSUBCRIBER OK";
    }
    else
    {
        printf("Invalid topic, cannot handle Unsubcribe\n");
        msg = "UNSUBCRIBER FAIL";
    }

    serverDoSendAck(cli, cli->income, msg);
}

void serverHandleDISCON(client *cli)
{
    mynet_close(cli->connection);
}

void *todoHandleClient(void *arg)
{
    client *cli = (client *)arg;
    broker *my_broker = cli->broker;
    while (1)
    {
        mes_recv(cli->connection, cli->income);
        if (cli->income->mes_type == DISCON)
        {
            serverHandleDISCON(cli);
            break;
        }
        switch (cli->income->mes_type)
        {
        case CON:
            serverHandleConnection(cli);
            break;
        case PUB:
            serverHandlePublisher(cli);
            break;
        case SUB:
            serverHandleSubscriber(cli);
            break;
        case UN_SUB:
            handleUnsubscribe(cli);
            break;

        default:
            printf("+error when read message type from client\n");
            break;
        }
    }
    pthread_detach(pthread_self());
    free(arg);
    return NULL;
}

int main(int argc, char *argv[])
{
    my_broker = initBroker(DEFAULT_ADDR, DEFAULT_PORT);
    client *my_client = NULL;
    pthread_t tid;

    while (1)
    {
        my_client = doBrokerAccept(my_broker);
        pthread_create(&tid, NULL, &todoHandleClient, (void *)my_client);
    }
}