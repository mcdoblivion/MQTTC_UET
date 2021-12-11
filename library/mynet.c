#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "mynet.h"

mqtt_connection *conn_init(const char *host, uint16_t port, net_status status)
{
    struct sockaddr_in *addr = (struct sockaddr_in *)mem__malloc(sizeof(struct sockaddr_in));

    if (addr != NULL)
    {
        memset(addr, 0, sizeof(addr));
        addr->sin_family = AF_INET;
        addr->sin_addr.s_addr = inet_addr(host);
        addr->sin_port = htons(port);
    }
    int connSock = socket(AF_INET, SOCK_STREAM, 0);
    if (connSock < 0)
    {
        printf("Error when creating socket\n");
        exit(1);
    }

    //create connection
    mqtt_connection *connection = (mqtt_connection *)malloc(sizeof(mqtt_connection));

    connection->sockfd = connSock;
    connection->addr = addr;
    connection->status = status;

    if (!connection)
        return NULL;
    else
        return connection;
}

void conn_free(mqtt_connection *connection)
{
    close(connection->sockfd);
    if (connection->addr)
    {
        free(connection->addr);
        connection->addr = NULL;
    }
    free(connection);
}

mqtt_connection *mynet_connect(const char *host, uint16_t port)
{
    mqtt_connection *connection = conn_init(host, port, CONNECTING);
    if (!connection)
    {
        printf("Error creating socket.");
        return NULL;
    }
    //procedure for connecting to server
    if (connect(connection->sockfd, (struct sockaddr *)connection->addr, sizeof(connection->addr)) < 0)
    {
        // error
        printf("Error when connecting!");
        conn_free(connection);
        return NULL;
    }

    //connect successfully
    connection->status = CONNECTED;
    printf("Connected to server mqtt %s:%d", inet_ntoa(connection->addr->sin_addr), ntohs(connection->addr->sin_port));

    return connection;
}

mqtt_connection *mynet_listen(const char *host, uint16_t port)
{
    mqtt_connection *connection = conn_init(host, port, CONNECTING);
    if (!connection)
    {
        printf("Error creating socket.");
        conn_free(connection);
        return NULL;
    }
    if(bind(connection->sockfd,(struct sockaddr*)connection->sockfd, sizeof(*(connection->addr))) < 0) {
        perror("bind");
        conn_free(connection);
        return NULL;
    }

    if(listen(connection->sockfd, 20) < 0) {
        perror("listen");
        conn_free(connection);
        return NULL;
    }

    printf("Broker is listening client on %s:%d\n", inet_ntoa(connection->addr->sin_addr), ntohs(connection->addr->sin_port));
    return connection;

}

mqtt_connection* mynet_accept(mqtt_connection* listener){
    struct sockaddr_in* clieAddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    if (!clieAddr) {
        return NULL;
    }

    socklen_t socklen = sizeof(*clieAddr);
    memset(clieAddr, 0, socklen);

    int cliSock = -1;
    while (cliSock < 0)
    {   
        cliSock = accept(listener->sockfd, (struct sockaddr *) clieAddr, (socklen_t *)&socklen);
        if(cliSock < 0){
            perror("accept");
            return NULL;
        }
    }
    
    printf("Broker has been initialized with client %s\n", inet_ntoa(clieAddr->sin_addr));

    mqtt_connection* connection = (mqtt_connection*)malloc(sizeof(mqtt_connection));
    connection->sockfd = cliSock;
    connection->addr = clieAddr;
    connection->status = CONNECTED;

    if(!connection){
        free(connection);
        return NULL;
    }
    return connection;

}

void conn_close(mqtt_connection* connection){
    if(connection->sockfd >0){
        printf("Closing connection with %s\n", inet_ntoa(connection->addr->sin_addr));
    }

    connection->sockfd = NULL;
}

void mynet_read(mqtt_connection* connection, void* recvBuf, size_t size){
    
    if(size <= 0)
        printf("error\n");
    
    int recvSize = read(connection->sockfd, recvBuf, size);
    
    if(recvSize < 0) {
        print("error");
    }
    else 
        printf("error\n");

}

void mynet_write(mqtt_connection* connection, void* recvBuf, size_t size){
    
    if(size <= 0)
        printf("error\n");
    
    int recvSize = write(connection->sockfd, recvBuf, size);
    
    if(recvSize < 0) {
        print("error");
    }
    else 
        printf("error\n");

}
