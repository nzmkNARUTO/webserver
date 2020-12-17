#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#include "socket.h"

int makeServerSocket(int port){
    int sock, optval;
    struct sockaddr_in serveraddr;

    //create sock
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){//use tcp protocol
        perror("Creat socket error\n");
        return -1;
    }

    //set socket option, allowing port reuse
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)) < 0){
        perror("Set socket opt error\n");
        return -1;
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);//use any address belonging to this machine
    serveraddr.sin_port = htons((unsigned short)port);

    //bind a socket to a port
    if(bind(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Bind error\n");
        return -1;
    }

    //listen a socket
    if(listen(sock, LISTENQ) <0 ){
        perror("Listen error\n");
        return -1;
    }

    return sock;
}