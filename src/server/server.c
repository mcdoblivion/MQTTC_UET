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
// #include "client.h"

#define DEFAULT_PORT 4445
#define DEFAULT_ADDR "127.0.0.1"

broker *my_broker = NULL;

void serverDoSendAck(client *cli, message *mesIn, char *msg)
{
    // message *mesOut = NULL;
    // message *mesOut = cli->outcome;
    // mes_ACK(mesOut, mesIn, msg);
    // client_send(cli, mesOut);
}

void serverHandleConnection(client *cli)
{
    //guiwr laij thoong tin vef broker
    printf("serverHandleConnection");
}

void serverHandlePublisher(client *cli)
{
    printf("serverHandleConnection");

    // message *in = cli->income;

    //find topic from mes income
    // broker find subs have topic above
    // create new mes to deliver: frame_MSG
    //using loop to detemine which client is subcriber in "subs" of broker
    // after that : doBrokerSendMessage(cli, sub, mes)
}

void serverHandleSubscriber(client *cli)
{
    printf("serverHandleConnection");

    //find topic in "income"->var_header
    // call client_sub(cli, topic_value)
    // call serverDoSendAck(client* cli, cli->income, char* msg)
}

void handleUnsubscribe(client *cli)
{
    printf("serverHandleConnection");
}

void *todoHandleClient(void *arg)
{
    client *cli = (client *)arg;
    broker *my_broker = cli->broker;
    while (1)
    {
        printf("loop\n"); 
        mes_recv(cli->connection, cli->income);
        printf("mes type %d\n", cli->income->mes_type);
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
            printf("error when read message type from client\n");
            exit (1);
        }
    }
    pthread_detach(pthread_self());
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