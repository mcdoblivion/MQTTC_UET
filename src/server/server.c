//this is file server
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

#define DEFAULT_PORT 4444
#define DEFAULT_ADDR "127.0.0.1"

broker *mybroker = NULL;

void serverDoSentAck(client *cli, message *mesIn, char *msg)
{
    // message *mesOut = NULL;
    // message *mesOut = cli->outcome;
    // mes_ACK(mesOut, mesIn, msg);
    // client_send(cli, mesOut);
}

void serverHandleCON(client *cli)
{
    //guiwr laij thoong tin vef broker
    printf("serverHandleCON");
}

void serverHandlePUB(client *cli)
{
    printf("serverHandleCON");

    // message *in = cli->income;

    //find topic from mes income
    // broker find subs have topic above
    // create new mes to deliver: frame_MSG
    //using loop to detemine which client is subcriber in "subs" of broker
    // after that : doBrokerSendMessage(cli, sub, mes)
}

void serverHandleSUB(client *cli)
{
    printf("serverHandleCON");

    //find topic in "income"->var_header
    // call client_sub(cli, topic_value)
    // call serverDoSentAck(client* cli, cli->income, char* msg)
}

void handleUNSUB(client *cli)
{
    //
    printf("serverHandleCON");
}

void *todoHandleClient(void *arg)
{
    client *cli = (client *)arg;
    broker *mybroker = cli->broker;
    printf("sdff"); //no
    while (1)
    {
        //client_read(cli); // read a client
        mes_recv(cli->connection, cli->income);
        switch (cli->income->mes_type)
        {
        case CON:
            serverHandleCON(cli);
            break;
        case PUB:
            serverHandlePUB(cli);
            break;
        case SUB:
            serverHandleSUB(cli);
            break;
        case UNSUB:
            handleUNSUB(cli);
            break;
        default:
            printf("error when read message type from client\n");
            break;
        }
    }
    pthread_detach(pthread_self());
}

int main(int argc, char *argv[])
{
    mybroker = initBroker(DEFAULT_ADDR, DEFAULT_PORT);
    client *myclient = NULL;
    pthread_t tid;

    while (1)
    {
        myclient = doBrokerAccept(mybroker);
        pthread_create(&tid, NULL, &todoHandleClient, (void *)myclient);
    }
}