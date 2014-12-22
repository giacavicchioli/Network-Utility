/**
 * Giacomo Cavicchioli
 * 0000654859
 * 
 * TCP_Stream_ServerCon.java
 */

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class TCP_Stream_ServerCon {
	public static final int PORT = 54321;

	public static void main(String[] args) throws IOException {

		int port = -1; // Porta sulla quale ascolta il server

		//controllo arogmenti
		try {
			if (args.length == 1) {
				port = Integer.parseInt(args[0]);
				// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) {
					System.out.println("Usage: 1024<serverPort<65535");
					port = PORT;
				}
				System.out.println("Using port: " + port);
			} else if (args.length == 0) {
				port = PORT;
				System.out.println("Using port: " + port);
			} else {
				System.out.println("Usage: java TCP_Stream_ServerCon or java TCP_Stream_ServerCon port");
				System.exit(1);
			}
		} catch (Exception e) {
			System.out.println("Error: ");
			e.printStackTrace();
			System.out.println("Usage: java TCP_Stream_ServerCon or java TCP_Stream_ServerCon port");
			System.exit(1);
		}

		//dichiarazione di oggetti utilizzati dal server per la comunicazione
		ServerSocket serverSocket = null;
		Socket clientSocket = null;
		
		//non dichiarare qua le variabili per la logica del programma, ma nel ChildServer
		
		//creazione server socket
		try {
			serverSocket = new ServerSocket(port);
			
			// se il demone deve ripartire in questo modo può farlo sullo stesso indirizzo
			serverSocket.setReuseAddress(true);
			
			System.out.println("TCP_Stream_ServerCon: avviato!");
			System.out.println("Server: creata la server socket: " + serverSocket);
			
		} catch (Exception e) {
			System.err.println("Error Server: problemi nella creazione della server socket: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}

		// Attesa di richieste bloccante finchè non arriva la prima
		try {

			while (true) {
				System.out.println("Server: in attesa di richieste...\n");

				try {
					clientSocket = serverSocket.accept(); // bloccante
					clientSocket.setSoTimeout(60000); // non indispensabile

					System.out.println("Server: connessione accettata: " + clientSocket);
				} catch (Exception e) {
					System.err.println("Server: problemi nella accettazione della connessione: " + e.getMessage());
					e.printStackTrace();
					continue; // il server continua a fornire il servizio ricominciando dall'inizio del ciclo
				}

				// Creazione e avvio del Server child
				try {
					new ChildServerThread(clientSocket).start();
				} catch (Exception e) {
					System.err.println("Server: problemi nel server thread: " + e.getMessage());
					e.printStackTrace();
					continue; // il server continua a fornire il servizio ricominciando dall'inizio del ciclo
				}
			}
		}
		// qui catturo le eccezioni non catturate all'interno del while in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			System.err.println("Error Server: ");
			e.printStackTrace();
			// chiusura di stream e socket
			System.out.println("TCP_Stream_ServerCon: termino...");
			System.exit(2);
		}
	}
}

/**
 * Thread lanciato per ogni richiesta accettata versione per il trasferimento di file binari
 */
class ChildServerThread extends Thread {

	private Socket clientSocket = null;

	// costruttore
	public ChildServerThread(Socket clientSocket) {
		this.clientSocket = clientSocket;
	}

	//Main invocabile con 0 o 1 argomenti. Argomento possibile -> porta su cui il server ascolta.
	public void run() {

		//dichiarazione di oggetti utilizzati dal server per la comunicazione
		DataInputStream inSock;
		DataOutputStream outSock;

		//dichiarare variabili per la logica del programma
		String richiesta;
		String esito;

		try {
			// creazione stream di input e out da socket
			inSock = new DataInputStream(clientSocket.getInputStream());
			outSock = new DataOutputStream(clientSocket.getOutputStream());
			
		} catch (IOException ioe) {
			System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
			ioe.printStackTrace();
			return;
		} catch (Exception e) {
			System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
			e.printStackTrace();
			return;
		}

		try	{
			//logica del server Child
			richiesta = inSock.readUTF();
			
			esito = richiesta + " " + richiesta;
			outSock.writeUTF(esito);
			
		}
		// qui catturo le eccezioni non catturate all'interno del while in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
			// chiusura di stream e socket
			System.out.println("Errore irreversibile, PutFileServerThread: termino...");
			System.exit(3);
		}
	}
}