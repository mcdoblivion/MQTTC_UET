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

#define DEFAULT_PORT 4445
#define DEFAULT_ADDR "127.0.0.1"

int pexit(const char *str)
{
    perror(str);
    exit(1);
}

mqtt_connection *clientDoConnect()
{
    mqtt_connection *con = mynet_connect("127.0.0.1", DEFAULT_PORT);
    if (!con)
        exit(1);

    message *mes = mes_new();
    if (!mes)
        exit(1);

    char *greeting = "I am Client";
    mes_CON(mes, (uint8_t *)greeting, strlen(greeting));
    //send CON
    mes_send(con, mes);

    //receive CONACK
    // mes_recv(con, mes);

    // printf("CONACK FROM BROKER: %s\n", mes->payload ? (char *)mes->payload : "CONNECT DISMISS");

    return con;
}

void clientDoDisconnect(mqtt_connection *con)
{
    mynet_close(con);
}

void clientDoPublish()
{
    mqtt_connection *conn = clientDoConnect();

    // message *inMes = mes_new();
    // message *outMes = mes_new();

    // // handle input topic for keyboard
    // char *topic = "home/bulb";
    // char data[] = "it is lower energy";
    // mes_PUB(outMes, topic, FLAG_PUB, NULL, strlen(data));

    // //send PUB
    // mes_send(con, outMes);
    // //receive ack
    // mes_recv(con, inMes);

    // printf("Published to \"%s\"\n", topic);

    // mes_free(outMes);
    // mes_free(inMes);
}

void clientDoSubscribe()
{
    mqtt_connection *conn = clientDoConnect();
    // message *inMes = mes_new();
    // message *outMes = mes_new();

    // // handle input topic for keyboard
    // char *topic = "home/bulb";
    // mes_SUB(outMes, FLAG_SUB, NULL, topic);

    // //SEND
    // mes_send(con, outMes);
    // //recv ack
    // mes_recv(con, inMes);

    // printf("Subcribbed topic: %s\n", topic);
}

int main(int agrc, char *argv[])
{
    char cmd[50] = "PUB";

    printf("Enter your command: ");
    gets(cmd);

    int todo = 0;
    if (strcmp(cmd, "PUB") == 0)
        todo = 1; // Publisher
    else if (strcmp(cmd, "SUB") == 0)
        todo = 2; // Subscriber

    switch (todo)
    {
    case 1:
        clientDoPublish();
        break;
    case 2:
        clientDoSubscribe();
        break;
    default:
        printf("Wrong command!");
        break;
    }
}