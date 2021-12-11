//this is file client
#include "broker.h"
#include "mqtt.h"
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

    char *greeting = "i am client";
    mes_CON(mes, (uint8_t *)greeting, strlen(greeting));

    //send CON
    mes_send(con, mes);

    //receive CONACK
    mes_recv(con, mes);

    printf("CONACK FROM BROKER: %s\n", mes->payload ? (char *)mes->payload : "CONNECT DISMISS");

    return con;
}

void clientDoDisconnect(mqtt_connection *con)
{
    mynet_close(con);
}
void clientDoPulish()
{
    mqtt_connection *con = clientDoConnect();

    message *inMes = mes_new();
    message *outMes = mes_new();

    // handle input topic for keyboard
    char *topic = "home/bulb";
    char data[] = "it is lower enerygy";
    mes_PUB(outMes, topic, FLAG_PUB, NULL, strlen(data));

    //send PUB
    mes_send(con, outMes);
    //receive ack
    mes_recv(con, inMes);

    printf("Published to \"%s\"\n", topic);

    mes_free(outMes);
    mes_free(inMes);
}

void clientDoSub()
{
    mqtt_connection *con = clientDoConnect();
    message *inMes = mes_new();
    message *outMes = mes_new();

    // handle input topic for keyboard
    char *topic = "home/bulb";
    mes_SUB(outMes, FLAG_SUB, NULL, topic);

    //SEND
    mes_send(con, outMes);
    //recv ack
    mes_recv(con, inMes);

    printf("Subcribed topic: %s\n", topic);
}

int main(int agrc, char *argv[])
{
    char cmd[50];
    printf("input your command: ");
    gets(cmd);

    int todo = 0;
    if (strcmp(cmd, "pub"))
        todo = 1; // pub
    else if (strcmp(cmd, "sub"))
        todo = 2; // sub

    switch (todo)
    {
    case 1:
        clientDoPulish();
        break;
    case 2:
        clientDoSub();
        break;
    default:
        printf("wrong");
        break;
    }
}