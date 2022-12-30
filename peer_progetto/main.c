#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include <string.h>
#include <errno.h>



struct Pacchetto{
    char nome_funzione[20];
    char porta[6];
    char parametri[15];
    char descrizione[30];
}pacchetto[3],ricezione[100], richiesta;

int somma (int a, int b){
    return a+b;
}

int sottrazione (int a, int b){
    return a-b;
}

char *troll(char *str){
    char stringa[100] = "ciriciao gente! ";
    return strcat(stringa,str);
}

int main(int argc, char *argv[])
{

    char buffer[4096];
    int socket_c_server, socketfd;
    struct sockaddr_in client_server, server_client, server;

    socket_c_server = Socket(AF_INET,SOCK_STREAM,0);

    client_server.sin_family = AF_INET;
    client_server.sin_port = htons(52000);



    inet_pton(AF_INET,"127.0.0.1",&client_server.sin_addr);

    Connect(socket_c_server,(struct sockaddr*)&client_server, sizeof(client_server));

    struct Pacchetto prova;
    int print = sizeof(prova);


    strcpy(pacchetto[0].nome_funzione,"somma");
    strcpy(pacchetto[0].porta,argv[1]);
    strcpy(pacchetto[0].parametri,"int int");
    strcpy(pacchetto[0].descrizione,"Somma di interi");
    strcpy(pacchetto[1].nome_funzione,"sottrazione");
    strcpy(pacchetto[1].porta,argv[1]);
    strcpy(pacchetto[1].parametri,"int int");
    strcpy(pacchetto[1].descrizione,"Sottrazione di interi");
    strcpy(pacchetto[2].nome_funzione,"troll");
    strcpy(pacchetto[2].porta,argv[1]);
    strcpy(pacchetto[2].parametri,"char *");
    strcpy(pacchetto[2].descrizione,"Concatena una stringa a...");


    ssize_t var = write(socket_c_server,&pacchetto,sizeof(pacchetto));
    puts("Contatto il server!\n");
    printf("scritti %d bytes\n",var);


    var = read(socket_c_server,&ricezione,sizeof(ricezione));
    puts("letto");
    if (var>=sizeof(ricezione[0]))
        for (int i=0; i<var/71; i++)
            printf("\n\nStruttura %d:\nNome:%s\nPorta:%s\nParametri:%s\nDesc:%s",i,ricezione[i].nome_funzione,ricezione[i].porta,ricezione[i].parametri,ricezione[i].descrizione);
    else
        puts("Sei il primo e unico peer connesso - Nessuna funzione disponibile!");





    int choice;
    puts("\n\nBenvenuto nel programma di gestione dei peer!\n");
    char str1[100],str2[100];

    fd_set readset,writeset;
    int fd, fd_occupati[32] = {0};
    int i,connectedfd,clientfd;
    char standard[10];
    sprintf(standard,"%d",1);
    strcat(standard,"\n");


    socketfd = Socket(AF_INET,SOCK_STREAM,0); //creo la socket per essere server

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));

    Bind(socketfd,(struct sockaddr*)&server,sizeof(server));
    Listen(socketfd,1024);

    FD_ZERO(&readset);
    FD_ZERO(&writeset);



    int maxfd = socketfd;
    while (1){

        FD_SET(socketfd, &readset);
        FD_SET(STDIN_FILENO, &readset);
        FD_SET(socket_c_server,&readset);


        for(i = socketfd; i <= maxfd; i++)
            if (fd_occupati[i])
                FD_SET(i,&writeset);


        puts("Sono qui\n");

        fd = Select(maxfd+1,&readset,&writeset,NULL,NULL);
        sleep(1);

        if (FD_ISSET(socketfd,&readset)){
            puts("Nuovo client connesso!\n");

            connectedfd = accept(socketfd,NULL,NULL);
            fd_occupati[connectedfd]++;

            if(maxfd < connectedfd)
                maxfd = connectedfd;

            fd--;

        }

        if (FD_ISSET(socket_c_server,&readset)){

            puts("sono nell'if socket\n");

                var = read(socket_c_server,&ricezione,sizeof(ricezione));
                if (var<71)
                    puts("Sei l'unico peer connesso - Nessuna funzione disponibile");
                else
                    for (int i=0; i<var/71; i++)
                        printf("\n\nStruttura %d:\nNome:%s\nPorta:%s\nParametri:%s\nDesc:%s",i,ricezione[i].nome_funzione,ricezione[i].porta,ricezione[i].parametri,ricezione[i].descrizione);

        }

        i = socketfd;

        if (FD_ISSET(STDIN_FILENO,&readset)){
            memset(buffer,0,sizeof(buffer));
            read(STDIN_FILENO, buffer, 10);

            if(strcmp(buffer,standard) == 0){
                fflush(stdin);
                write(socket_c_server," ",strlen(" "));
                puts("prima dell'if... credo");


            }
            else{

                fflush(stdin);

                puts("Sei un client! Scegli, dalla lista precedente, l'indice corrispondente alla funzione che vuoi richiedere al peer: ");
                scanf("%d",&choice);

                fflush(stdin);

                clientfd =  Socket(AF_INET,SOCK_STREAM,0);

                client_server.sin_port = htons(atoi(ricezione[choice].porta));

                Connect(clientfd,&client_server,sizeof(client_server)); //modificare la porta con la porta della struct

                puts("Connesso ad un altro peer!");

                write(clientfd,&ricezione[choice],sizeof(ricezione[choice])); //mando la richiesta del client

                read(clientfd,buffer,4096); //leggo la risposta del server per la conferma

                puts(buffer);

                puts("Inserire i parametri di input della funzione richiesta, avendo cura di distanziarli con uno spazio:");

                gets(str1);

                write(clientfd,str1,strlen(str1));

                memset(buffer,0,sizeof(buffer));

                memset(str1,0,sizeof(str1));

                read(clientfd,buffer,4096);

                puts("L'output desiderato: ");
                printf("%s\n",buffer);

                shutdown(clientfd,2);
                close(clientfd);
                fflush(stdin);

                fd--;
            }

        }

        while (fd > 0 && i < 32){

            i++;

            if(!fd_occupati[i])
                continue;

            if (FD_ISSET(i,&writeset)){

                printf("Servo il client %desimo\n",i);

                fd--;

                read(i,&richiesta,sizeof(richiesta));

                if(strcmp(richiesta.nome_funzione,"somma") == 0){
                    int a,b;
                    char *token1, *token2;
                    write(i,"Hai richiesto la funzione di somma!",strlen("Hai richiesto la funzione di somma!"));
                    read(i,str2,100);
                    token1 = strtok(str2," ");
                    token2 = strtok(token1, "\n");
                    a = atoi(token1);
                    b = atoi(token2);
                    a = somma(a,b);
                    memset(str2,0,sizeof(str2));
                    sprintf(str2,"%d",a);
                    write(i,str2,strlen(str2));
                }

                close(i);

                fd_occupati[i] = 0;

                FD_CLR(i,&writeset);
                FD_CLR(i,&readset);

                printf("Client %desimo servito\n",i);

                if(maxfd == i)
                    while (fd_occupati[--i] == 0){
                        maxfd = i;
                        break;
                    }



            }

        }


    }



    return 0;
}
