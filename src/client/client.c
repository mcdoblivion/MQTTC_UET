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

#define DEFAULT_PORT 4448
#define DEFAULT_ADDR "127.0.0.1"
#define LENGTH 2048

int pexit(const char *str)
{
    perror(str);
    exit(1);
}

mqtt_connection *clientDoConnect()
{
    mqtt_connection *con = mynet_connect(DEFAULT_ADDR, DEFAULT_PORT);
    if (!con)
        exit(1);

    message *mes = mes_new();
    if (!mes)
        exit(1);

    char *greeting = "I am Client";
    mes_CON(mes, (uint8_t *)greeting, strlen(greeting));
    //send CON
    mes_send(con, mes);

    // receive CONACK
    message *mesACK = mes_new();
    mes_recv(con, mesACK);

    printf("CONACK FROM BROKER: %s\n", mesACK->payload ? (char *)mesACK->payload : "CONNECT DISMISS");

    return con;
}

void clientDoDisconnect(mqtt_connection *con)
{
    printf("client handle disconnect\n");
    message *mes = mes_new();
    if (!mes)
        exit(1);

    char *payload = "DISCONNECT";
    mes_DISCON(mes, (uint8_t *)payload, strlen(payload));
    //send CON
    mes_send(con, mes);

    mynet_close(con);
    exit(1);
}

void clientDoPublish(mqtt_connection *con)
{
    message *inMes = mes_new();
    message *outMes = mes_new();

    // // handle input topic for keyboard
    // printf("input your topic and data: ");
    // char input[50];
    // gets(input);

    char *topic = "home/light";
    char *data = "it is lower energy";
    mes_PUB(outMes, topic, FLAG_PUB, data, strlen(data));

    // //send PUB
    mes_send(con, outMes);
    // //receive ack
    mes_recv(con, inMes);
    // if(strcmp(inMes->variable_header, "PUBACK")==0 && strcmp(inMes->payload, "PUBLISH OK") == 0)
    if (strcmp(inMes->payload, "PUBLISH OK") == 0)
        printf("Published to broker \"%s\"\n", topic);
    else
    {
        printf("Published to broker fail\n");
    }
    mes_free(outMes);
    mes_free(inMes);
}

void clientDoSubscribe(mqtt_connection *con)
{
    message *inMes = mes_new();
    message *outMes = mes_new();

    // handle input topic for keyboard
    // char *topic = "home/bulb";
    char *payload = "home/light";
    mes_SUB(outMes, FLAG_SUB, (uint8_t *)payload, strlen(payload));

    //SEND
    mes_send(con, outMes);
    //recv ack
    mes_recv(con, inMes);

    if (strcmp(inMes->payload, "SUBCRIBER OK") == 0)
    {
        printf("Subcribbed topic: \"%s\"\n", outMes->payload);
        while (1)
        {
            /* code */
            printf("listening........\n");
            message *listenMes = mes_new();
            mes_recv(con, listenMes);
            printf("RECEIVED DATA: \"%s\"\n", listenMes->payload);
        }
    }

    else
    {
        printf("SUBCRIBER to broker fail\n");
    }
}

void clientDoUnsubcribe(mqtt_connection *myConnection)
{
    printf("client handle unsub\n");
}

void send_msg_handler(void *arg)
{
    mqtt_connection *myConnection = (mqtt_connection *)arg;
    char cmd[50];
    int todo = 0;
    char message[2048] = {};
    // while (1)
    // {
    //     printf("Enter your command with broker: ");
    //     gets(cmd);
    //     if (strcmp(cmd, "PUB") == 0)
    //         todo = 1; // Publisher
    //     else if (strcmp(cmd, "SUB") == 0)
    //         todo = 2; // Subscriber
    //     else if (strcmp(cmd, "UNSUB") == 0)
    //         todo = 3;
    //     else if (strcmp(cmd, "DISCON") == 0)
    //         todo = 4;

    //     switch (todo)
    //     {
    //     case 1:
    //         clientDoPublish(myConnection);
    //         break;
    //     case 2:
    //         clientDoSubscribe(myConnection);
    //         break;
    //     case 3:
    //         clientDoUnsubcribe(myConnection);
    //         break;
    //     case 4:
    //         clientDoDisconnect(myConnection);
    //         break;
    //     default:
    //         printf("Wrong command!\n");
    //         break;
    //     }
    // }
    // catch_ctrl_c_and_exit(2);
}

void recv_msg_handler(void * arg)
{
    mqtt_connection* con= (mqtt_connection*) arg;
    while (1)
    {
        message *listenMes = mes_new();
        mes_recv(con, listenMes);
        printf("->>Data received: \'%s\', from topic: \'%s\'\n", listenMes->payload, listenMes->variable_header);
        mes_free(listenMes);

    }
}
int main(int agrc, char *argv[])
{
    mqtt_connection *myConnection = NULL;
    char cmd[50];
    int todo = 0;

    while (1)
    {
        printf("Enter your command: ");
        gets(cmd);
        if (strcmp(cmd, "CON") == 0)
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
        else
        {
            printf("wrong command, pls try\n");
        }
    }
    while (1)
    {
        printf("Enter your command with broker: ");
        gets(cmd);
        if (strcmp(cmd, "PUB") == 0)
            todo = 1; // Publisher
        else if (strcmp(cmd, "SUB") == 0)
            todo = 2; // Subscriber
        else if (strcmp(cmd, "UNSUB") == 0)
            todo = 3;
        else if (strcmp(cmd, "DISCON") == 0)
            todo = 4;

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
    }

    // pthread_t send_msg_thread;
    // if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, myConnection) != 0)
    // {
    //     printf("ERROR: pthread\n");
    //     exit(1);
    // }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, (void*)myConnection) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    return 0;
}