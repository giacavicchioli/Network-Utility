//Giacomo Cavicchioli
//mat. 0000654859
//template: RPC_Server.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <rpc/rpc.h>
#include "sala.h"



/* STATO SERVER */
static Output state;
static int inizializzato=0;

void inizializza()	{
	int i, j;

	if( inizializzato==1 )
		return;

	//inizializzazione struttura dati
	
	//termine inizializzazione
	inizializzato = 1;
	printf("Terminata inizializzazione struttura dati!\n");
}



int * funzione_uno_1_svc(Input *input, struct svc_req *rqstp)		{
	static int result=-1;
	result=-1;
	inizializza();

	//logica prima funzione

}

Output * funzione_due_1_svc(void *in, struct svc_req *rqstp)	{
	inizializza();
	
	return (&state);
}

