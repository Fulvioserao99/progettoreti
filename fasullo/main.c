#include <stdio.h>
#include <stdlib.h>

#include "functions.h"
typedef struct{
    char *nome_funzione;
    char *porta;
    char *parametri;
    char *descrizione;
}Pacchetto;


struct ared{
    char nome_funzione[20];
    char porta[6];
    char parametri[15];
    char descrizione[30];
}magicarp[300],type;

int main(int argc, char **argv)
{
    int socketfd, listenfd,n;
    char buffer[4096];



    Pacchetto *pacchetto = malloc(2*sizeof(Pacchetto));
    pacchetto[0].nome_funzione = malloc(strlen("somma"));
    strcpy(pacchetto[0].nome_funzione,"somma");
    pacchetto[0].porta = malloc(strlen("ared"));
    strcpy(pacchetto[0].porta,"ared");
    pacchetto[0].parametri = malloc(strlen("int int"));
    strcpy(pacchetto[0].parametri,"int int");
    pacchetto[0].descrizione = malloc(strlen("Somma di interi"));
    strcpy(pacchetto[0].descrizione,"Somma di interi");

    if ((socketfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        exit(1);
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(52000);

    if ((bind(socketfd,(struct sockaddr*)&server,sizeof(server))) < 0){
        perror("bind");
        exit(1);
    }

    if (listen(socketfd,5) < 0){
        perror("listen");
        exit(1);
    }
    int index = 0;

    for(; ;){
        if ((listenfd = accept(socketfd,(struct sockaddr*)NULL,NULL)) < 0){
            perror("accept");
            exit(1);

        }
        puts("accepted\n");

        ssize_t var = read(listenfd,&magicarp[index],sizeof(magicarp));

        printf("letti %d bytes",var);
        index += var/71;
        var = write(listenfd,&magicarp,(sizeof(type)*(index)));
        printf("scritti %d bytes",var);

        /*puts(magicarp[0].nome_funzione);
        puts(magicarp[1].porta);
        puts(magicarp[1].descrizione);
        puts(magicarp[0].parametri);*/

        close(listenfd);




    }
    close(socketfd);

        exit(100);

    return 0;
}
