//Giacomo Cavicchioli
//mat. 0000654859
//template: Select_Client_Stream.c

/*
  due versioni possibili:
  - una connessione per tutte le richieste (questo template)
    vedi: esercitazione 3 (avanzata) - stream_client2.c
  - una connessione per ogni richiesta
    vedi: esercitazione 3 (avanzata) - stream_client1.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>  

#define DIM_BUFF 100
#define LENGTH_FILE_NAME 20

// dichiarazione eventuali funzioni
bool checkPortVal(char *a)  {
  int aux = 0;
  while( a[aux]!= '\0' )  {
      if((a[aux] < '0') || (a[aux] > '9'))  {
        return false;
      }
      aux++;
  }

  return true;
}

int main(int argc, char *argv[])  { // main client stream
  int sd, nread, port;
  struct hostent *host;
  struct sockaddr_in servaddr;

  // variabili per la logica del programma
  char c, ok, nome_remote_file[LENGTH_FILE_NAME], nome_local_file[LENGTH_FILE_NAME];
  int fd_local;

  // controllo del numero di argomenti
  if(argc!=3) {
    printf("Using: %s serverAddress serverPort\n", argv[0]);
    exit(1);
  }

  // controllo della porta inserita
  if(!checkPortVal(argv[2]))  {
    printf("Using: %s serverAddress serverPort\n", argv[0]);
    perror("Second value must be an integer\n");
    exit(2);
  } else  {
    port = atoi(argv[2]);
  }

  // inizializzazione indirizzo Server
  memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  host = gethostbyname(argv[1]);
  if(host == NULL)  {
    printf("%s not found in /etc/hosts\n", argv[1]);
    exit(1);
  }
  servaddr.sin_addr.s_addr=((struct in_addr*) (host->h_addr))->s_addr;
  servaddr.sin_port = htons(atoi(argv[2]));
  
  printf("Client Stream - launched!");

  // creazione Socket
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd<0)  {
    perror("opening socket");
    exit(1);
  }
  printf("Client: creata la socket sd: %d\n", sd); //stampo a video il valore del file descriptor della socket

  // BIND della Socket implicita nella connect
  if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))<0)  {
    perror("connect");
    exit(1);
  }
  printf("Client Strem: bind socket done\n");


  // LOGICA DEL PROGRAMMA
  printf("\nAny key to proceed, EOF to terminate...\n");

  printf("\n--------\nRemote filename: ");
  while (gets(nome_remote_file)) {
    
    if (write(sd, nome_remote_file, (strlen(nome_remote_file)+1))<0)  {
      perror("write");
      break;
    }
    printf("\tRemote file selected: __%s__\n", nome_remote_file);

    if (read(sd, &ok, 1)<0) {
      perror("read");      
      break;
    }

    if(ok=='S')  {
      printf("Local file name: ");
      gets(nome_local_file);
      printf("\tLocal file selected: __%s__\n", nome_local_file);
      
      if((fd_local = open(nome_local_file, O_WRONLY | O_CREAT, 0777))<0) {
        perror("open file sorgente");
        printf("\n--------\nRemote filename: ");
        continue;
      } else  {
        printf("\tReceiving remote file __%s__ and saving into __%s__...\n", nome_remote_file, nome_local_file);
        while((nread=read(sd,&c,1))>0)  { // leggo a caratteri per individuare il fine file
          if (c!='\0')
            write(fd_local,&c,1);
          else
            break;
        }

        if(nread < 0) {
          perror("read");
          break;
        }

        printf("\tDONE!\n");
      }

    } else { //if(ok=='N')  {
      printf("\nError: file not found on remote server\n");
    }

    printf("\n--------\nRemote filename: ");

  }

  /* ----------- Libero risorse ----------- */
  close(sd);

  printf("\nClient: termino...\n");
  
  

  exit(0);
  
}

