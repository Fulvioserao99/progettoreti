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

int lunghezza(char *str) {
return strlen(str)-1;
}

int modulo(int a, int b) {
return a%b;
}

char *hacked(char *str){
    char *stringa = "Sei stato hackerato da Fulvio/Luca/Simone!";
    return stringa;
}

int main(int argc, char *argv[])
{
    int size_struct = sizeof(ricezione[0]);
    char buffer[4096];
    int socket_c_server, socketfd;
    struct sockaddr_in client_server, server;

    socket_c_server = Socket(AF_INET,SOCK_STREAM,0);

    client_server.sin_family = AF_INET;
    client_server.sin_port = htons(52000);



    inet_pton(AF_INET,"127.0.0.1",&client_server.sin_addr);

    Connect(socket_c_server,(struct sockaddr*)&client_server, sizeof(client_server));


    strcpy(pacchetto[0].nome_funzione,"lunghezza");
    strcpy(pacchetto[0].porta,argv[1]);
    strcpy(pacchetto[0].parametri,"char *");
    strcpy(pacchetto[0].descrizione,"Restituisce lunghezza della stringa");
    strcpy(pacchetto[1].nome_funzione,"modulo");
    strcpy(pacchetto[1].porta,argv[1]);
    strcpy(pacchetto[1].parametri,"int int");
    strcpy(pacchetto[1].descrizione,"Resto della divisione tra due int");
    strcpy(pacchetto[2].nome_funzione,"hacked");
    strcpy(pacchetto[2].porta,argv[1]);
    strcpy(pacchetto[2].parametri,"char *");
    strcpy(pacchetto[2].descrizione,"Stringa a sorpresa stampata a schermo!");


    ssize_t var = write(socket_c_server,&pacchetto,sizeof(pacchetto));
    puts("Contatto il server!\n");



    var = read(socket_c_server,&ricezione,sizeof(ricezione));

    if (var>=size_struct)
        for (int i=0; i<var/size_struct; i++)
            printf("\n\nStruttura %d:\nNome:%s\nPorta:%s\nParametri:%s\nDesc:%s\n",i,ricezione[i].nome_funzione,ricezione[i].porta,ricezione[i].parametri,ricezione[i].descrizione);
    else
        puts("Sei il primo e unico peer connesso - Nessuna funzione disponibile!");





    int choice;
    puts("\n\nBenvenuto nel programma di gestione dei peer!\n");

    char str1[100],str2[100];

    fd_set readset,writeset;
    int fd, fd_occupati[FD_SETSIZE] = {0};
    int i,connectedfd,clientfd;
    char standard[10];
    sprintf(standard,"%d",1);
    strcat(standard,"\n");


    socketfd = Socket(AF_INET,SOCK_STREAM,0); //creo la socket per essere server

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));

    setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int));

    Bind(socketfd,(struct sockaddr*)&server,sizeof(server));
    Listen(socketfd,1024);

    FD_ZERO(&readset);
    FD_ZERO(&writeset);

    puts("\nPremere 1 ed inviare per aggiornare la lista delle funzioni disponibili,\n o invio per richiedere una funzione ad un qualsiasi client\n");

    int maxfd = socketfd;
    while (1){

        FD_SET(socketfd, &readset);
        FD_SET(STDIN_FILENO, &readset);
        FD_SET(socket_c_server,&readset);


        for(i = socketfd; i <= maxfd; i++)
            if (fd_occupati[i])
                FD_SET(i,&writeset);




        fd = Select(maxfd+1,&readset,&writeset,NULL,NULL);
        sleep(1);

        if (FD_ISSET(socketfd,&readset)){
            puts("Nuovo client connesso!\n");

            connectedfd = accept(socketfd,NULL,NULL);
            fd_occupati[connectedfd]++;

            if(maxfd < connectedfd)
                maxfd = connectedfd;

            fd--;
            fflush(stdin);
        }

        if (FD_ISSET(socket_c_server,&readset)){


                system("clear");
                var = read(socket_c_server,&ricezione,sizeof(ricezione));
                if (var > 0 && var < size_struct)
                    puts("Sei l'unico peer connesso - Nessuna funzione disponibile");

                else if (var >= size_struct)
                    for (int i=0; i<var/size_struct; i++)
                        printf("\n\nStruttura %d:\nNome:%s\nPorta:%s\nParametri:%s\nDesc:%s\n",i,ricezione[i].nome_funzione,ricezione[i].porta,ricezione[i].parametri,ricezione[i].descrizione);

                else{
                    puts("La connessione col server e' stata interrotta - Chiusura applicazione");
                    exit(0);
                }
        }

        i = socketfd;

        if (FD_ISSET(STDIN_FILENO,&readset)){
            memset(buffer,0,sizeof(buffer));
            read(STDIN_FILENO, buffer, 10);

            if(strcmp(buffer,standard) == 0){
                fflush(stdin);
                write(socket_c_server," ",strlen(" "));



            }
            else if (strcmp(buffer, "\n") == 0){

                fflush(stdin);


                puts("Sei un client! Scegli, dalla lista precedente, l'indice corrispondente alla funzione che vuoi richiedere al peer: ");
                choice = -1;


                scanf("%d",&choice);

                system("clear");

                fflush(stdin);

                clientfd =  Socket(AF_INET,SOCK_STREAM,0);

                client_server.sin_port = htons(atoi(ricezione[choice].porta));

                Connect(clientfd,&client_server,sizeof(client_server)); //modificare la porta con la porta della struct

                puts("Connesso ad un altro peer!");

                write(clientfd,&ricezione[choice],sizeof(ricezione[choice])); //mando la richiesta del client

                read(clientfd,buffer,4096); //leggo la risposta del server per la conferma

                puts(buffer);

                puts("\nInserire i parametri di input della funzione richiesta, avendo cura di distanziarli con uno spazio, qualora fossero piu' di uno:");

                read(0,str1,30);

                write(clientfd,str1,strlen(str1));

                memset(buffer,0,sizeof(buffer));

                memset(str1,0,sizeof(str1));

                read(clientfd,buffer,4096);

                puts("\nL'output desiderato: ");
                printf("%s\n",buffer);

                shutdown(clientfd,2);
                close(clientfd);
                fflush(stdin);

                fd--;
            }
            else{
                system("clear");
                puts("- Digitazione non valida -\n");
                puts("\nPremere 1 ed inviare per aggiornare la lista delle funzioni disponibili,\n o invio per richiedere una funzione ad un qualsiasi client\n");
                continue;
            }


        }

        while (fd > 0 && i < FD_SETSIZE){

            i++;

            if(!fd_occupati[i])
                continue;

            if (FD_ISSET(i,&writeset)){



                fd--;

                read(i,&richiesta,size_struct);

                printf("E' stata richiesta la funzione: %s\n",richiesta.nome_funzione);

                
              if(strcmp(richiesta.nome_funzione,"lunghezza") == 0){
                    int a;
                    write(i,"Hai richiesto la funzione di lunghezza!",strlen("Hai richiesto la funzione di lunghezza!"));
                    read(i,str2,100);
                    a = lunghezza(str2);
                    memset(str2,0,sizeof(str2));
                    sprintf(str2,"%d",a);
                    write(i,str2,strlen(str2));
                }

               if(strcmp(richiesta.nome_funzione,"modulo") == 0){
                    int a,b;
                    char *token1, *token2;
                    write(i,"Hai richiesto la funzione di modulo!",strlen("Hai richiesto la funzione di modulo!"));
                    read(i,str2,100);
                    token1 = strtok(str2," ");
                    token2 = strtok(NULL, " ");
                    a = atoi(token1);
                    b = atoi(token2);
                    a = modulo(a,b);
                    memset(str2,0,sizeof(str2));
                    sprintf(str2,"%d",a);
                    write(i,str2,strlen(str2));
                }

                   if(strcmp(richiesta.nome_funzione,"hacked") == 0){
                    write(i,"Hai richiesto la funzione hacked!",strlen("Hai richiesto la funzione hacked!"));
                    read(i,str2,100);
                    strcpy(str2,hacked(str2));
                    write(i,str2,strlen(str2));
                }

                fflush(stdin);

                close(i);

                fd_occupati[i] = 0;

                FD_CLR(i,&writeset);
                FD_CLR(i,&readset);

                printf("Client servito!\n");
                fflush(stdin);
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
