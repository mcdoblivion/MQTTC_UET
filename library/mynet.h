#ifndef _MYNET_H_
#define _MYNET_H_

#include "mqtt.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef enum
{
    INIT = 0,
    LISTEN,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    CLOSE,
} network_status;

struct connection
{
    int sockfd;
    struct sockaddr_in *addr;
    network_status status;
};

mqtt_connection *mynet_listen(const char *host, uint16_t port);
mqtt_connection *mynet_connect(const char *host, uint16_t port);
mqtt_connection *mynet_accept(mqtt_connection *listenter);
void mynet_close(mqtt_connection *connection);
void mynet_read(mqtt_connection *connection, void *buf, size_t size);
void mynet_write(mqtt_connection *connection, void *buf, size_t size);
void mynet_close(mqtt_connection *connection);

#endif