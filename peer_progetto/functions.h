#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>




int Socket(int family, int type, int protocol){
    int n;
    if ((n = socket(family, type, protocol)) < 0){
        perror("socket");
        exit(1);
    }
    return(n);

}



void Connect (int socket, struct sockaddr* sock_addr, int len){

    if ((connect(socket,sock_addr,(socklen_t)len)) < 0) {
        fprintf(stderr,"connect error\n");
        exit(1);
    }
}


void Bind(int socket, struct sockaddr* sock_addr, int len){
    if ( (bind(socket,sock_addr,(socklen_t)len)) < 0 ) {
        perror("bind");
        exit(1);
    }
}


void Listen(int socket, int len){
    if ((listen(socket, len)) < 0 ){
        perror("listen");
        exit(1);
    }
}

int Accept(int socket,struct sockaddr* sock_addr, int len){
    int n;
    if ((n = accept(socket,sock_addr, (socklen_t)len)) < 0 ) {
        perror("accept");
        exit(1);
    }
    return n;
}




int Select(int maxfdp,fd_set *readset,fd_set *writeset,fd_set *excpset,struct timeval *timeout){

    int n;

    if ((n = select(maxfdp,readset,writeset,excpset,timeout))< 0){
                    puts("error select");
                    exit(-1);
    }

    return n;
}
