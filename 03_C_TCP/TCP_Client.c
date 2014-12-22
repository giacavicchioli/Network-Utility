//Giacomo Cavicchioli
//mat. 0000654859
//template: TCP_Client.c

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

#define FILENAME_SIZE 20
#define DIM_BUFF 256

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

int main(int argc, char *argv[])  { //main client
	//variabili per la comunicazione
	struct hostent *host;
	struct sockaddr_in servaddr;
	int port, sd, nread;
	char buff[DIM_BUFF];

	//variabili per la logica del programma
	char nome_sorg[FILENAME_SIZE];
	char ok;
	int fd_sorg;

	// controllo del numero degli argomenti
	if(argc!=3) {
		printf("Error: %s serverAddress serverPort\n", argv[0]);
		exit(1);
	}

	// controllo della porta inserita
	if(!checkPortVal(argv[2]))  {
		printf("Secondo argomento non intero\n");
		printf("Error: %s serverAddress serverPort\n", argv[0]);
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

	// CLIENT BODY
	// Struttura: FILTRO. Ciclo di accettazione rihcieste da utente
	// si invia un file al server che lo salva in remoto cambiando nome

	printf("\t----------\nNome del file da spedire al server, EOF per terminare: ");
	while (gets(nome_sorg)) {
		printf("File da aprire: %s\n", nome_sorg); // DEBUG

		// Verifico l'esistenza del file
		if((fd_sorg=open(nome_sorg, O_RDONLY))<0) {
			perror("open file sorgente");
			printf("\t----------\nNome del file da spedire al server, EOF per terminare: ");
			continue;
		}

		// creazione Socket Stream
		sd = socket(AF_INET, SOCK_STREAM, 0);
		if(sd<0)  {
			perror("apertura socket");
			exit(1);
		}
		printf("Client: creata la socket sd: %d\n", sd); //stampo a video il valore del file descriptor della socket

		// BIND della Socket implicita nella connect
		if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))<0)  {
			perror("connect");
			exit(1);
		}
		printf("Client: connect ok\n");


		//invio il contenuto del file
		printf("\nClient: invio file al server\n");
		while((nread = read(fd_sorg, buff, DIM_BUFF))>0)  {
			//write(1, buff, nread);
			write(sd, buff, nread);
		}

		printf("Client: file inviato\n");

		// Chiusura socket in spedizione -> invio dell'EOF
		shutdown(sd,1);

		// Chiusura socket in ricezione
		shutdown(sd, 0);

		// libero risorse
		close(fd_sorg);
		close(sd);

		printf("\t----------\nNome del file da spedire al server, EOF per terminare: ");
	}

	printf("\nClient: termino...\n");
	exit(0);

}
