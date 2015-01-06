//Giacomo Cavicchioli
//mat. 0000654859
//template: RPC_Client.c

#include <stdio.h>
#include <rpc/rpc.h>
#include "sala.h"

#define LUNGHFILA 7
#define NUMFILE 10

// dichiarazione eventuali funzioni

main (int argc, char *argv[])	{	// main client datagram
	char *host;
	CLIENT *cl;

	//variabili per la logica del programma
	int *ris, *start_ok;
	void * in;
	Output *output;
	Input input;
	char str[5];
	int intero;
	int  i, j;
	char c, ok[256];

	//controllo del numero di argomenti
	if (argc != 2)	{
		printf("Using: %s serverAddress\n", argv[0]);
		exit (1);
	}
	host = argv[1];

	cl = clnt_create(host, PROGRAM, PROGRAMVERS, "udp");
	if (cl == NULL)	{
		clnt_pcreateerror (host);
		exit (1);
	}


	//inizio della logica del programma
	printf("Inserire:\nU) per prima funzione\tD) per seconda funzinoe\t^D per terminare: ");
	while (gets (ok))	{

		if( strcmp(ok, "U")==0 )	{
			//prima funzione

			//logica per recuperare la scelta specifica
			printf("Carattere (P, D o B): \n");
			gets(ok);
			
			// Leggo il carattere
			while((strcmp(ok, "P")!=0) && (strcmp(ok, "D")!=0) && (strcmp(ok, "B")!=0))	{
				printf("Lettera sbagliata! Inserisci P, D o B: \n");
				gets(ok);
			}
			input.a=ok[0];

			// Leggo l'intero
			intero=-1;
			while( intero<0 || intero>(NUMFILE-1) ){
				printf("Intero (da 0 a %i): \n", (NUMFILE-1));
				while (scanf("%i", &fila) != 1)	{
					//lettura completa del buffer per sbloccare la scanf ferma in caratteri sbagliati
					do {
						c=getchar(); printf("%c ", c);
					} while
						(c!= '\n');
					printf("Fila: ");
				}
			}
			//Consumo fine linea
			gets(ok);
			input.b=fila;

			// Invocazione remota
			ris = funzione_uno(&input, cl);
			if(ris == NULL)	{
				clnt_perror(cl, host);
				exit(1);
			}
			if(*ris<0)
				printf("Problemi nell'esecuzione della prenotazione\n");
			else
				printf("Prenotazione effettuata con successo\n");
		}	else if( strcmp(ok,"D")==0 )	{
			//seconda funzione

			output = visualizza_stato_1(in,cl);
			if(output == NULL) { clnt_perror(cl, host); exit(1); }
			printf("Output:\n");
				
			//stampa dell'output


		} else
			printf("Errore!\n");

		printf("Inserire:\nU) per prima funzione\tD) per seconda funzinoe\t^D per terminare: ");
	}

	// Libero le risorse, distruggendo il gestore di trasporto
	clnt_destroy (cl);
	exit (0);

}