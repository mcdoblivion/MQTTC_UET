/**
 * @file client.c
 * 
 * Client program
 * Nguyen Minh Thang
 * Nguyen Hong Thai
 * Tran Nguyen Phuong Nam
 * Dong Minh Cuong
 */
#include "mqtt.h"
#include "broker.h"
#include "message.h"
#include "mynet.h"
#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 4444
#define DEFAULT_ADDR "127.0.0.1"
#define LENGTH 2048
volatile sig_atomic_t flag = 0;

int pexit(const char *str)
{
    perror(str);
    exit(1);
}
void catch_ctrl_c_and_exit(int sig)
{
    flag = 1;
}
mqtt_connection *clientDoConnect()
{
    mqtt_connection *con = mynet_connect(DEFAULT_ADDR, DEFAULT_PORT);

    if (!con)
        exit(1);

    message *mes = mes_new();
    if (!mes)
        exit(1);

    char *greeting = "HELLO BROKER";
    mes_CON(mes, (uint8_t *)greeting, strlen(greeting));
    //send CON
    mes_send(con, mes);

    // receive CONACK
    message *mesACK = mes_new();
    mes_recv(con, mesACK);

    printf(">>RESPONSE FROM BROKER: %s\n", mesACK->payload ? (char *)mesACK->payload : "CONNECT DISMISS");

    mes_free(mes);
    mes_free(mesACK);

    return con;
}

void clientDoDisconnect(mqtt_connection *con)
{
    message *mes = mes_new();
    if (!mes)
        exit(1);

    char *payload = "DISCONNECT";
    mes_DISCON(mes, (uint8_t *)payload, strlen(payload));
    //send DISCON
    mes_send(con, mes);

    mes_free(mes);
    mynet_close(con);
    exit(1);
}

void clientDoPublish(mqtt_connection *con)
{
    message *inMes = mes_new();
    message *outMes = mes_new();

    // handle input topic for keyboard
    printf("Switch mode to publisher, pls enter your topic and data!\n");
    char topic[30];

    char data[30];
    printf("Enter topic: ");
    scanf("%s", topic);
    getchar();
    printf("Enter data: ");
    fflush(stdin);
    // fgets(data, sizeof(data), stdin);
    gets(data);
    // char *topic = "home/light";
    // char *data = "it is lower energy";

    //create message PUB
    mes_PUB(outMes, topic, FLAG_PUB, data, strlen(data));

    //send PUB
    mes_send(con, outMes);
    //receive ack
    mes_recv(con, inMes);
    if (strcmp(inMes->payload, "PUBLISH OK") == 0)
        printf(">>Successfully published message= \"%s\", topic =\"%s\"\n", data, topic);
    else
    {
        printf(">>Published to broker fail\n");
    }

    mes_free(outMes);
    mes_free(inMes);
}

void clientDoSubscribe(mqtt_connection *con)
{
    message *inMes = mes_new();
    message *outMes = mes_new();

    // handle input topic for keyboard
    printf("Switch mode to subcriber, pls enter your topic need to subcribe!\n");
    char topic[30];
    printf("Enter topic: ");
    scanf("%s", topic);
    getchar();
    // char *topic = "home/#";
    mes_SUB(outMes, flag_UN_SUB, (uint8_t *)topic, strlen(topic));

    //send SUB
    mes_send(con, outMes);
    //recv ack
    mes_recv(con, inMes);

    if (strcmp(inMes->payload, "SUBCRIBER OK") == 0)
    {
        printf(">>Successfully Subcribbed topic: \"%s\"\n", outMes->payload);
    }
    else
    {
        printf(">>SUBCRIBER to broker fail\n");
    }

    mes_free(outMes);
    mes_free(inMes);
}

void clientDoUnsubcribe(mqtt_connection *con)
{
    message *inMes = mes_new();
    message *outMes = mes_new();

    // handle input topic for keyboard
    printf("Switch mode to unsubcriber, pls enter your topic need to unsubcribe!\n");
    char topic[30];
    printf("Enter topic: ");
    scanf("%s", topic);
    getchar();
    // char *payload = "home/light";
    mes_UNSUB(outMes, FLAG_UN_SUB_ACK, (uint8_t *)topic, strlen(topic));

    //SEND
    mes_send(con, outMes);
    //recv ack
    mes_recv(con, inMes);

    if (strcmp(inMes->payload, "UNSUBCRIBER OK") == 0)
    {
        printf(">>UNSUBCRIBBED topic: \"%s\"\n", outMes->payload);
    }
    else
    {
        printf(">>Unsubcibe fail\n");
    }

    mes_free(outMes);
    mes_free(inMes);
}

void send_msg_handler(void *arg)
{
    mqtt_connection *myConnection = (mqtt_connection *)arg;
    char cmd[50];
    int todo = 0;
    while (1)
    {
        printf("Client# ");
        gets(cmd);
        if (strcmp(cmd, "publish") == 0)
            todo = 1; // Publisher
        else if (strcmp(cmd, "subscribe") == 0)
            todo = 2; // Subscriber
        else if (strcmp(cmd, "unsubscribe") == 0)
            todo = 3;
        else if (strcmp(cmd, "disconnect") == 0)
            todo = 4;
        else if (strcmp(cmd, "") == 0)
            continue;

        switch (todo)
        {
        case 1:
            clientDoPublish(myConnection);
            break;
        case 2:
            clientDoSubscribe(myConnection);
            break;
        case 3:
            clientDoUnsubcribe(myConnection);
            break;
        case 4:
            clientDoDisconnect(myConnection);
            break;
        default:
            printf("Wrong command!\n");
            break;
        }
        todo = 0;
    }
    catch_ctrl_c_and_exit(2);
}

void recv_msg_handler(void *arg)
{
    mqtt_connection *con = (mqtt_connection *)arg;
    while (1)
    {
        message *listenMes = mes_new();
        mes_recv(con, listenMes);
        if (con->status == DISCONNECTED)
            break;

        printf("\n=>>New message received: \'%s\', from topic: \'%s\'\n", listenMes->payload, listenMes->variable_header);
        // getchar();
        mes_free(listenMes);
    }
    catch_ctrl_c_and_exit(2);
}
int main(int argc, char *argv[])
{
    mqtt_connection *myConnection = NULL;
    char cmd[50];
    int todo = 0;
    signal(SIGINT, catch_ctrl_c_and_exit);

    while (1)
    {
        printf("Client> ");
        gets(cmd);
        if (strcmp(cmd, "connect") == 0)
        {
            myConnection = clientDoConnect();
            if (myConnection->status != CONNECTED)
            {
                printf("fail to connect, try again\n");
            }
            else
            {
                break;
            }
        }
        else if(strcmp(cmd, "") == 0)
        {
            continue;
        }
        else
        {
            printf("wrong command, pls try\n");
        }
    }

    // thread for send message to broker
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, myConnection) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    // thread for receive message from broker
    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, (void *)myConnection) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }
    while (1)
    {
        if (flag)
        {
            printf("\nBye\n");
            break;
        }
    }
    clientDoDisconnect(myConnection);

    return EXIT_SUCCESS;
}