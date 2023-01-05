#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

struct Pacchetto{
    char nome_funzione[20];
    char porta[6];
    char parametri[15];
    char descrizione[30];
}storage[300];


int fix_memory(int index, int offset, int deleted, struct Pacchetto* storage){
    for(int i=offset+deleted; i<index; i++){
        memcpy(&storage[i-deleted],&storage[i],sizeof(storage[i]));
    }
    index-=deleted;
    memset(&storage[index],0,deleted);
    return index;

}

int main()
{
    int size_struct = sizeof(storage[0]);
    int socketfd, listenfd, i;
    char buffer[4096];
    char ricezione[4096];


    socketfd = Socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in server;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(52000);

    Bind(socketfd,(struct sockaddr*)&server,sizeof(server));

    Listen(socketfd,50);




    int index = 0;
    int n_count;
    int porta1,porta2,porta3,deleted,offset;


    fd_set readset;
    FD_ZERO(&readset);

    int fd_open[FD_SETSIZE]={0}, fd, maxfd,fd_connected;
    fd_open[socketfd]++;
    maxfd = socketfd;
    while(1){

        for (int i=socketfd; i<=maxfd; i++)
            if(fd_open[i])
                FD_SET(i,&readset);


        fd = select(maxfd+1,&readset,NULL,NULL,NULL);

        sleep(1);

        


        if(FD_ISSET(socketfd,&readset)){

            puts("Nuovo client connesso!");
            fd_connected = accept(socketfd,NULL,NULL);


            n_count = 0; //azzero il contatore per tenere traccia degli elementi inviabili al peer iesimo
            ssize_t var = FullRead(fd_connected,&storage[index],sizeof(storage));

            index+=var/size_struct; //aggiorno l'indice per le locazioni occupate nella struct storage

            porta1 = atoi(storage[index-1].porta);

            fd_open[fd_connected] = porta1;

            for(int i=0; i<index; i++){
                porta2 = atoi(storage[i].porta);
                if(porta1 != porta2 && (strcmp(storage[i].porta,"") != 0) ){
                    FullWrite(fd_connected,&storage[i],size_struct);
                    n_count++;

                }
            }

            if(!n_count)
                FullWrite(fd_connected," ",strlen(" "));

            sleep(1);




            if (maxfd < fd_connected)
                maxfd = fd_connected;
            fd--;


            }







        i = socketfd;



        while(fd > 0 && i < FD_SETSIZE){
            i++;
            if(fd_open[i] == 0)
                continue;

            if(FD_ISSET(i,&readset)){
                fd--;
                ssize_t bytes = FullRead(i,buffer,sizeof(buffer));
                if(bytes < 0){

                    puts("read error");
                    exit(0);
                }
                else if(bytes == 0){

                    printf("Client con la porta %d disconnesso\n", fd_open[i]);

                    deleted=0;
                    offset=0;

                    for(int j=0; j<index; j++){
                        porta3 = atoi(storage[j].porta);
                        if (porta3 == fd_open[i]){
                            memset(&storage[j],0,size_struct);
                            deleted++; //incrementa il contatore degli elementi cancellati
                            if(deleted == 1)
                                offset = j; //indice dal quale iniziare la cancellazione
                        }

                    }

                    index = fix_memory(index,offset,deleted,storage);
                    fd_open[i] = 0;
                    FD_CLR(i,&readset);
                    close(i);
                    if (maxfd == i)
                        while (fd_open[--i] == 0){
                            maxfd = i;
                            break;
                }
                }
                else{

                    n_count = 0;

                    for(int j=0; j<index; j++){
                        porta1 = fd_open[i];
                        porta2 = atoi(storage[j].porta);
                        if(porta1 != porta2 && (strcmp(storage[j].porta,"") != 0) ){
                            FullWrite(i,&storage[j],sizeof(storage[j]));
                            n_count++;
                        }



                    }


                    if (!n_count)
                        FullWrite(i," ",strlen(" "));





                }
            }



        }

    }


    return 0;
}
