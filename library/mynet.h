#ifndef _MYNET_H_
#define _MYNET_H_

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

// enum status{
//     LISTEN,
//     CONNECTING,
//     CONNECTED,
//     RECONNECTING,
//     DISCONNECTED,
//     CLOSE

// }




// struct mqtt_connection {
//     int sockfd;
//     struct sockaddr_in* addr;
//     status
// }

// mqtt_connection do_connect
// mqtt_connection do_listen
// mqtt_connection do_accept
// void destroy
// void close()

// status__err net__read(mqtt_connection* conn, void* buf, ssize_t size, ssize_t* n_read, uint8_t block);
// status__err net__write(mqtt_connection* conn, void* buf, ssize_t size, ssize_t* n_write, uint8_t block);



#endif