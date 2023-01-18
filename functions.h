#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

struct Pacchetto{
    char nome_funzione[20];
    char porta[6];
    char parametri[15];
    char descrizione[30];
};


int fix_memory(int index, int offset, int deleted, struct Pacchetto* storage){
    for(int i=offset+deleted; i<index; i++){
        memcpy(&storage[i-deleted],&storage[i],sizeof(storage[i]));
    }
    index-=deleted;
    memset(&storage[index],0,deleted);
    return index;

}



ssize_t FullWrite(int fd, const void *buf, size_t count)
{
	size_t nleft;
	ssize_t nwritten;
	nleft = count;
	while (nleft > 0) {

	
	if ( (nwritten = write(fd, buf, nleft)) < 0) {
		if (errno == EINTR) { 
		continue;
		} else {
		exit(nwritten); 
		}
	}

	nleft -= nwritten;
	buf +=nwritten;
	}

	return (nleft);
}

ssize_t FullRead(int fd, void *buff, size_t count) {
    size_t nleft;
    ssize_t nread;
    nleft = count;
    while(nleft > 0) {
        nread = read(fd, buff, nleft);
        if(errno == EINTR) {
            continue;
        }
        else {
            return nread;
        }
        if(nread == 0) {
            break;
        }
        nleft -= nread;
        buff += nread;
    }
    buff = 0;
    return nleft;
}

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

int Accept(int socket,struct sockaddr* sock_addr, socklen_t *len){
    int n;
    if ((n = accept(socket,sock_addr, len)) < 0 ) {
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
