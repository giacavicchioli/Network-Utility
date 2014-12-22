//Giacomo Cavicchioli
//mat. 0000654859
//template: TCP_Server.c

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

#define FILENAME_SIZE 20
#define BUFFER_SIZE 1024

/* ----------- dichiarazione eventuali funzioni ----------- */
void gestore(int signo) {
	int stato;
	printf("Esecuzione gestore di SIGCHLD\n");
	wait(&stato);
}


int main(int argc, char **argv) { // main server
	//variabili per la comunicazione
	struct sockaddr_in cliaddr, servaddr;
	struct hostent *host;
	const int on = 1;
	int  listen_sd, conn_sd;
	int port, len, nread;
	int defaultPort = 54321;

	//variabili per la logica del programma
	char buffer[BUFFER_SIZE], nome_file[FILENAME_SIZE];
	int fd_dest;

	// controllo del numero degli argomenti
	if(argc == 1)	{
		port = defaultPort;
	} else if(argc == 2) {
		port = atoi(argv[1]);

		if (port < 1024 || port > 65535)  {
			printf("Porta fuori range...");
			port = defaultPort;
		}
	}
	printf("Using port: %d", port);

	// inizializzazione indirizzo Server
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);


	// creazione Socket
	listen_sd=socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sd <0)  {
		perror("creazione socket ");
		exit(1);
	}
	printf("Server: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n", listen_sd);

	// opzioni Socket
	if(setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)  {
		perror("set opzioni socket d'ascolto");
		exit(1);
	}
	printf("Server: set opzioni socket d'ascolto ok\n");

	// bind Socket
	if(bind(listen_sd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
		perror("bind socket d'ascolto");
		exit(1);
	}
	printf("Server: bind socket d'ascolto ok\n");

	// listen
	if (listen(listen_sd, 5)<0) {
		perror("listen");
		exit(1);
	}
	printf("Server: listen ok\n");

	signal(SIGCHLD, gestore); // Aggancio gestore per evitare figli zombie

	// ciclo di ricezione Richieste
	for(;;) {
		printf("\n---- server ready ----\n");
		len=sizeof(cliaddr);
		if((conn_sd=accept(listen_sd,(struct sockaddr *)&cliaddr,&len))<0)  {
			if (errno==EINTR) {
				perror("Forzo la continuazione della accept");
				continue;
			} else
				exit(1);
		}

		if (fork()==0)  {

			// FIGLIO
			close(listen_sd);
			host=gethostbyaddr((char *) &cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
			if (host == NULL) {
				printf("Server (child) - client host information not found\n");
				continue;
			} else
				printf("Server (child) - host client: %s \n", host->h_name);

			printf("Server (child): ho creato correttamente il file %s, ora ci salvo il contenuto mandato dal client.\n", nome_file);

			if((fd_dest=open("backup.txt", O_WRONLY | O_CREAT, 0777))<0) {
				perror("open file destinazione");
				exit(1);
			}

			while ((nread = read(conn_sd, buffer, BUFFER_SIZE)) > 0) {
				write(fd_dest, buffer, nread);
			}

			// libero risorse
			close(conn_sd);
		}

		// PADRE
		close(conn_sd);
	}

	printf("\nServer: termino...\n");
	exit(0);
}
