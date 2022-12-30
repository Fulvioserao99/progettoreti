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
    for(int i=offset+deleted; i<(index-offset); i++){
        memcpy(&storage[i-deleted],&storage[i],sizeof(storage[i]));
    }
    index-=deleted;
    memset(&storage[index],0,sizeof(storage));
    return index;

}

int main()
{
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

    int fd_open[32]={0}, fd, maxfd,fd_connected;
    fd_open[socketfd]++;
    maxfd = socketfd;
    while(1){

        for (int i=socketfd; i<=maxfd; i++)
            if(fd_open[i])
                FD_SET(i,&readset);


        fd = select(maxfd+1,&readset,NULL,NULL,NULL);


        sleep(1);

        ssize_t length;


        if(FD_ISSET(socketfd,&readset)){

            puts("Nuovo client connesso!");
            fd_connected = accept(socketfd,NULL,NULL);


            n_count = 0; //azzero il contatore per tenere traccia degli elementi inviabili al peer iesimo
            ssize_t var = read(fd_connected,&storage[index],sizeof(storage));

            index+=var/sizeof(storage[0]); //aggiorno l'indice per le locazioni occupate nella struct storage

            porta1 = atoi(storage[index-1].porta);

            fd_open[fd_connected] = porta1;

            for(int i=0; i<index; i++){
                porta2 = atoi(storage[i].porta);
                if(porta1 != porta2 && (strcmp(storage[i].porta,"") != 0) ){
                    write(fd_connected,&storage[i],sizeof(storage[0]));
                    n_count++;

                }
            }

            if(!n_count)
                write(fd_connected," ",strlen(" "));





            if (maxfd < fd_connected)
                maxfd = fd_connected;
            fd--;


            }







        i = socketfd;



        while(fd > 0 && i < 32){
            i++;
            if(fd_open[i] == 0)
                continue;

            if(FD_ISSET(i,&readset)){
                fd--;
                ssize_t bytes = read(i,buffer,sizeof(buffer));
                if(bytes < 0){

                    puts("read error");
                    exit(0);
                }
                else if(bytes == 0){
                    deleted=0;
                    offset=0;

                    for(int j=0; j<index; j++){
                        porta3 = atoi(storage[j].porta);
                        if (porta3 == fd_open[i]){
                            memset(&storage[j],0,sizeof(storage[0]));
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
                            write(i,&storage[j],sizeof(storage[j]));
                            n_count++;
                        }



                    }


                    if (!n_count)
                        write(i," ",strlen(" "));





                }
            }



        }

    }


    return 0;
}
