//Giacomo Cavicchioli
//mat. 0000654859
//template: Select_Client_Datagram.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>

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

int main(int argc, char **argv) { // main client datagram
  struct hostent *host;
  struct sockaddr_in clientaddr, servaddr;
  int  sd, len, port;
  
  //variabili per la logica del programma
  int num_file;
  char nome_dir[LENGTH_FILE_NAME];

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
  
  // inizializzazione indirizzo client
  memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
  clientaddr.sin_family = AF_INET;
  clientaddr.sin_addr.s_addr = INADDR_ANY;
  clientaddr.sin_port = 0;
  
  // inizializzazione indirizzo server
  memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  host = gethostbyname(argv[1]);
  if (host == NULL) {
    printf("%s not found in /etc/hosts\n", argv[1]);
    exit(2);
  } else  {
    servaddr.sin_addr.s_addr = ((struct in_addr *) (host->h_addr))->s_addr;
    servaddr.sin_port = htons(port);
  }

  printf("Client Datagram - launched!");


  // creazione socket
  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0)  {
    perror("opening socket");
    exit(1);
  }
  printf("Client: creata la socket sd: %d\n", sd); //stampo a video il valore del file descriptor della socket

  // BIND della Socket a una porta scelta dal sistema
  if(bind(sd,(struct sockaddr *) &clientaddr, sizeof(clientaddr))<0)  {
    perror("bind socket");
    exit(1);
  }
  printf("Client Datagram: bind socket done, port: %i\n", clientaddr.sin_port);

  // LOGICA DEL PROGRAMMA
  printf("\nAny key to proceed, EOF to terminate...\n");
  
  printf("\n--------\nFolder name: ");
  while (gets(nome_dir))  {
    printf("File counter requested, remote folder selected: %s\n", nome_dir);

    // invio nome del direttorio
    len=sizeof(servaddr);
    if(sendto(sd, nome_dir, (strlen(nome_dir)+1), 0, (struct sockaddr *)&servaddr, len)<0) {
      perror("sendto");
      printf("\n--------\nFolder name: ");
      continue; // se questo invio fallisce il client torna all'inzio del ciclo
    }


    // ricezione del risultato
    printf("Waiting for result from server, please be patient...\n");
    if (recvfrom(sd, &num_file, sizeof(num_file), 0, (struct sockaddr *)&servaddr, &len)<0) {
      perror("recvfrom");
      printf("\n--------\nFolder name: ");
      continue; // se questa ricezione fallisce il client torna all'inzio del ciclo
    }

    printf("File count received: %ld\n", ntohl(num_file));

    printf("\n--------\nFolder name: ");
  }

  /* ----------- libero le risorse ----------- */
  close(sd);

  printf("\nClient Datagram terminate...\n");
  exit(0);
}
