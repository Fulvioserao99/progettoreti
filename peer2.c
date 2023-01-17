#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include <string.h>
#include <errno.h>


int moltiplicazione(int a, int b) {
return a*b;
}

float divisione(float a, float b) {
if(a >= b)
return a/b;
else
return 0.0;
}

char *kvaratskhelia(char *str){
    char *stringa = "Kvaratskhelia è indubbiamente più forte di Rafael Leao!";
    return stringa;
}

int main(int argc, char *argv[])
{
    struct Pacchetto pacchetto[3],ricezione[100], richiesta;
    int size_struct = sizeof(ricezione[0]);
    char buffer[4096];
    int socket_c_server, socketfd;
    struct sockaddr_in client_server, server;

    socket_c_server = Socket(AF_INET,SOCK_STREAM,0);

    client_server.sin_family = AF_INET;
    client_server.sin_port = htons(52000);



    inet_pton(AF_INET,"127.0.0.1",&client_server.sin_addr);

    Connect(socket_c_server,(struct sockaddr*)&client_server, sizeof(client_server));



    strcpy(pacchetto[0].nome_funzione,"moltiplicazione");
    strcpy(pacchetto[0].porta,argv[1]);
    strcpy(pacchetto[0].parametri,"int int");
    strcpy(pacchetto[0].descrizione,"Moltiplicazione di interi");
    strcpy(pacchetto[1].nome_funzione,"divisione");
    strcpy(pacchetto[1].porta,argv[1]);
    strcpy(pacchetto[1].parametri,"float float");
    strcpy(pacchetto[1].descrizione,"Divisione di float");
    strcpy(pacchetto[2].nome_funzione,"kvaratskhelia");
    strcpy(pacchetto[2].porta,argv[1]);
    strcpy(pacchetto[2].parametri,"char *");
    strcpy(pacchetto[2].descrizione,"No descrizione, FATTI!");


    ssize_t var = FullWrite(socket_c_server,&pacchetto,sizeof(pacchetto));
    puts("Contatto il server!\n");






    int choice;
    puts("\n\nBenvenuto nel programma di gestione dei peer!\n");

    char str1[100],str2[100];

    fd_set readset,writeset;
    int fd, fd_occupati[FD_SETSIZE] = {0};
    int i,connectedfd,clientfd;
    char standard[10];
    sprintf(standard,"%d",1);
    strcat(standard,"\n");


    int number = 0;
    char buff_bytes[10];

    socketfd = Socket(AF_INET,SOCK_STREAM,0); //creo la socket per essere server

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));

    setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int));

    Bind(socketfd,(struct sockaddr*)&server,sizeof(server));
    Listen(socketfd,1024);



    puts("\nLe funzioni disponibili degli altri peer saranno visibili tra 3 secondi:\n");
    sleep(3);

    int maxfd = socketfd;
    while (1){

	FD_ZERO(&readset);
    	FD_ZERO(&writeset);
    	FD_SET(socketfd, &readset);
        FD_SET(STDIN_FILENO, &readset);
        FD_SET(socket_c_server,&readset);


        for(i = socketfd; i <= maxfd; i++)
            if (fd_occupati[i])
                FD_SET(i,&writeset);




        fd = Select(maxfd+1,&readset,&writeset,NULL,NULL);
        

        if (FD_ISSET(socketfd,&readset)){
            puts("Nuovo client connesso!\n");

            connectedfd = accept(socketfd,NULL,NULL);
            fd_occupati[connectedfd]++;

            if(maxfd < connectedfd)
                maxfd = connectedfd;

            fd--;
            
        }

        if (FD_ISSET(socket_c_server,&readset)){
		number = 0;
		memset(buff_bytes,0,sizeof(buff_bytes));
		fd--;
		var = FullRead(socket_c_server,buff_bytes,sizeof(buff_bytes));
		if (var == 0){
			puts("La connessione col server e' stata interrotta - Chiusura applicazione");
                	exit(0);
		}
		number = atoi(buff_bytes);
		FullWrite(socket_c_server," ",strlen(" "));
		system("clear");
		if (number){
			recv(socket_c_server,&ricezione,number*size_struct,MSG_WAITALL); //chiama la recv con la flag waitall - aspetta l'arrivo di tutti i bytes
			
			for (int i=0; i<number; i++)
                        	printf("\n\nStruttura %d:\nNome:%s\nPorta:%s\nParametri:%s\nDesc:%s\n",i,ricezione[i].nome_funzione,ricezione[i].porta,ricezione[i].parametri,ricezione[i].descrizione);
		}
		else
			puts("Sei l'unico peer connesso - Nessuna funzione disponibile");
               	    	
               	puts("\nPremere 1 ed inviare per aggiornare la lista delle funzioni disponibili,\n o invio per richiedere una funzione ad un qualsiasi client\n");
		
	}

        i = socketfd;

        if (FD_ISSET(STDIN_FILENO,&readset)){
            memset(buffer,0,sizeof(buffer));
            read(STDIN_FILENO, buffer, 10);

            if(strcmp(buffer,standard) == 0){
                fflush(stdin);
                FullWrite(socket_c_server," ",strlen(" "));



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

                Connect(clientfd,(struct sockaddr*)&client_server,sizeof(client_server)); //modificare la porta con la porta della struct

                puts("Connesso ad un altro peer!");

                FullWrite(clientfd,&ricezione[choice],sizeof(ricezione[choice])); //mando la richiesta del client

                FullRead(clientfd,buffer,4096); //leggo la risposta del server per la conferma

                puts(buffer);

                puts("\nInserire i parametri di input della funzione richiesta, avendo cura di distanziarli con uno spazio, qualora fossero piu' di uno:");

                read(0,str1,30);

                FullWrite(clientfd,str1,strlen(str1));

                memset(buffer,0,sizeof(buffer));

                memset(str1,0,sizeof(str1));

                FullRead(clientfd,buffer,4096);

                puts("\nL'output desiderato: ");
                printf("%s\n",buffer);

                shutdown(clientfd,2);
                close(clientfd);
                

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

                FullRead(i,&richiesta,size_struct);

                printf("E' stata richiesta la funzione: %s\n",richiesta.nome_funzione);

                if(strcmp(richiesta.nome_funzione,"moltiplicazione") == 0){
                    int a,b;
                    char *token1, *token2;
                    write(i,"Hai richiesto la funzione di moltiplicazione!",strlen("Hai richiesto la funzione di moltiplicazione!"));
                    read(i,str2,10);
                    token1 = strtok(str2," ");
                    token2 = strtok(NULL, " ");
                    a = atoi(token1);
                    b = atoi(token2);
                    a = moltiplicazione(a,b);
                    memset(str2,0,sizeof(str2));
                    sprintf(str2,"%d",a);
                    write(i,str2,strlen(str2));
                }

               if(strcmp(richiesta.nome_funzione,"divisione") == 0){
                    int a,b;
                    char *token1, *token2;
                    write(i,"Hai richiesto la funzione di divisione!",strlen("Hai richiesto la funzione di divisione!"));
                    read(i,str2,100);
                    token1 = strtok(str2," ");
                    token2 = strtok(NULL, " ");
                    a = atoi(token1);
                    b = atoi(token2);
                    a = divisione(a,b);
                    memset(str2,0,sizeof(str2));
                    sprintf(str2,"%d",a);
                    write(i,str2,strlen(str2));
                }

                   if(strcmp(richiesta.nome_funzione,"kvaratskhelia") == 0){
                    write(i,"Hai richiesto la funzione Kvaratskhelia!",strlen("Hai richiesto la funzione Kvaratskhelia!"));
                    read(i,str2,100);
                    strcpy(str2,kvaratskhelia(str2));
                    write(i,str2,strlen(str2));
                }

                

                close(i);

                fd_occupati[i] = 0;

                FD_CLR(i,&writeset);
                FD_CLR(i,&readset);

                printf("Client servito!\n");
                
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
