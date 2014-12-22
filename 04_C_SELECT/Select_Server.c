//Giacomo Cavicchioli
//mat. 0000654859
//template: Select_Server.c

/*
  due versioni possibili:
  - una connessione per tutte le richieste (questo template)
    vedi: esercitazione 3 (avanzata) - select_server2.c
  - una connessione per ogni richiesta
    vedi: esercitazione 3 (avanzata) - select_server1.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define DIM_BUFF 100
#define LENGTH_FILE_NAME 20
#define max(a,b) ((a) > (b) ? (a) : (b))


// dichiarazione eventuali funzioni

int conta_file (char *name) {

  //known issue:
  //  - include in count also . and ..

  DIR *dir;
  struct dirent * dd;
  int count = 0;
  dir = opendir (name);
  
  while ((dd = readdir(dir)) != NULL) {
    printf("Found file: %s\n", dd-> d_name);
    count++;
  }

  printf("Total count calculated: %d\n", count);
  closedir (dir);
  return count;
}

void gestore(int signo) {
  int stato;
  printf("launching SIGCHLD handler\n");
  wait(&stato);
}

int main(int argc, char **argv) { // server MAIN
  struct sockaddr_in cliaddr, servaddr;
  int  listenfd, connfd, udpfd, fd_file, nready, maxfdp1;
  const int on = 1;
  fd_set rset; //maschera
  int len, nread, nwrite, num, ris, port;

  //variabili per la logica del programma
  char zero=0, buff[DIM_BUFF], nome_file[LENGTH_FILE_NAME], nome_dir[LENGTH_FILE_NAME];

  // Controllo del numero degli argomenti
  if(argc!=2) {
    printf("Error: %s port\n", argv[0]);
    exit(1);
  } else
    port = atoi(argv[1]); //manca il controllo della porta
  


  printf("Server - launched!\n");




  // creazione socket TCP
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd <0)  {
    perror("opening stream socket");
    exit(1);
  }
  printf("Stream socket - created stream listen socket corretly, fd: %d\n", listenfd);

  // creazione, settaggio e opzioni Socket
  memset ((char *)&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0) {
    perror("set opzioni socket TCP");
    exit(2);
  }
  printf("Stream socket - options set correctly\n");

  if (bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
    perror("bind socket TCP");
    exit(3);
  }
  printf("Stream socket - bind done correctly\n");

  if (listen(listenfd, 5)<0)  {
    perror("listen");
    exit(4);
  }
  printf("Stream socket - listen set correctly\n");


  // creazione socket UDP
  udpfd=socket(AF_INET, SOCK_DGRAM, 0);
  if(udpfd <0)  {
    perror("opening UDP socket");
    exit(5);
  }
  printf("Dgram socket - socket created correctly, fd=%d\n", udpfd);

  // inizializzazione indirizzo Server e Bind
  memset ((char *)&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  if(setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)  {
    perror("set opzioni socket UDP");
    exit(6);
  }
  printf("Dgram socket - options set correctly\n");

  if(bind(udpfd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
    perror("bind socket UDP");
    exit(7);
  }
  printf("Dgram socket - bind done correctly\n");

  // aggancio gestore per evitare figli zombie
  signal(SIGCHLD, gestore);
  printf("Server - SIGCHILD handler attached\n");

  // pulizia e settaggio maschera dei FileDescriptor
  FD_ZERO(&rset);
  maxfdp1=max(listenfd, udpfd)+1;
  printf("Server - filedescriptor mask set\n");

  // ciclo di ricezione eventi dalla Select
  for(;;) {
    FD_SET(listenfd, &rset);
    FD_SET(udpfd, &rset);

    if ((nready=select(maxfdp1, &rset, NULL, NULL, NULL))<0)  {
      if (errno==EINTR)
        continue;
      else {
        perror("select");
        exit(8);
      }
    }


    // GESTIONE RICHIESTE STREAM
    if (FD_ISSET(listenfd, &rset))  {
      printf("Server - received stream request.\n");

      len = sizeof(struct sockaddr_in);
      if((connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&len))<0)  {
        if (errno==EINTR)
          continue;
        else  {
          perror("accept");
          exit(9);
        }
      }

      // processo figlio che serve la richiesta di operazione
      if (fork()==0)  { 
        close(listenfd);
        printf("Server - CHILD - pid=%i\n", getpid());

        for (;;)  { 
          int readRes;		
          if((readRes=read(connfd, &nome_file, sizeof(nome_file)))<0) {
            perror("read");
            break;
          } else if(readRes==0) {  // abbiamo raggiunto la EOF
            printf("received EOF\n");
            break;
          }
          printf("Server - CHILD %i - requested file: %s\n", getpid(), nome_file);

          fd_file=open(nome_file, O_RDONLY);
          if (fd_file<0)  {
            printf("requested file error\n");
            write(connfd, "N", 1);
          } else  {
            write(connfd, "S", 1);

            // lettura dal file (a blocchi) e scrittura sulla socket
            printf("Server - CHILD - start reading and sending file...\n");
            while((nread=read(fd_file, buff, sizeof(buff)))>0)  {
              if((nwrite=write(connfd, buff, nread))<0) {
                perror("write");
                break;
              }
            }
            printf("Server - CHILD %i - File sent\n", getpid());
            
            // invio al client segnale di terminazione: zero binario
            write(connfd, &zero, 1);
            close(fd_file);
          }
        }

        printf("Server - CHILD %i: chiudo connessione e termino\n", getpid());

        // libero risorse CHILD
        close(connfd);
        exit(0);
      }

      // libero risorse PADRE
      close(connfd);

    }


    // GESTIONE RICHIESTE UDP
    if (FD_ISSET(udpfd, &rset)) {
      printf("Server - received dgram request.\n");

      len=sizeof(struct sockaddr_in);
      if (recvfrom(udpfd, &nome_dir, sizeof(nome_dir), 0, (struct sockaddr *)&cliaddr, &len)<0) {
        perror("recvfrom");
        continue;
      }

      printf("Server - requested file count for folder: %s\n", nome_dir);
      num = conta_file(nome_dir);
      printf("Server - count result: %i\n", num);

      ris=htonl(num);
      
      if (sendto(udpfd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len)<0)  {
        perror("sendto");
        continue;
      }
    }
  }

  printf("Server - terminate...");
  exit(1);

}

