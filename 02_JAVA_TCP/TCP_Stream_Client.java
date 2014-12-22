//Giacomo Cavicchioli
//mat. 0000654859
//template: TCP_Stream_Client.java

import java.net.*;
import java.io.*;

public class TCP_Stream_Client {
	public static void main(String[] args) throws IOException {

		// dichiarazione endopoint di comunicazione ottenuti dai parametri
		InetAddress addr = null;
		int port = -1;

		//controllo arogmenti
		try {
			if (args.length == 2) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
			} else {
				System.out.println("Usage: java TCP_Stream_Client serverAddr serverPort");
				System.exit(1);
			}
		} catch (Exception e) {
			System.out.println("Error: ");
			e.printStackTrace();
			System.out.println("Usage: java PutFileClient serverAddr serverPort");
			System.exit(2);
		}

		
		//dichiarazione oggetti utilizzati dal client per la comunicazione
		Socket socket = null;
		DataInputStream inSock = null; //stream di input su socket
		DataOutputStream outSock = null; ////stream di output su socket
		
		//dichiarare variabili per la logica del programma
		String richiesta = "ciao";
		String esito;
		
		//creazione socket
		try {
			socket = new Socket(addr, port);
			socket.setSoTimeout(30000);
			System.out.println("Creata la socket: " + socket);
		} catch (Exception e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1); //se in un ciclo continue;
		}

		//creazione stream di I/O su socket
		try {
			inSock = new DataInputStream(socket.getInputStream());
			outSock = new DataOutputStream(socket.getOutputStream());
		} catch (IOException e) {
			System.out.println("Problemi nella creazione degli stream su socket: ");
			e.printStackTrace();
			System.exit(1); //se in un ciclo continue;
		}
		
		//invio stream di input e chiusura socket in output
		try {
			outSock.writeUTF(richiesta);
			
			// Scrittura sulla outSock dello Stream in output
			// NB: per file usare: FileUtility.trasferisci_a_byte_file_binario(new DataInputStream(inFile), outSock);
			
			// chiudo la socket in upstream
			socket.shutdownOutput();
			System.out.println("Trasmissione terminata.");
		} catch (SocketTimeoutException ste) {
			System.out.println("Timeout scattato: ");
			ste.printStackTrace();
			socket.close();
			System.exit(1); //se in un ciclo continue;
		} catch (Exception e) {
			System.out.println("Error: ");
			e.printStackTrace();
			socket.close();
			System.exit(1); //se in un ciclo continue;
		}

		//eventuale ricezione esito dal server e chiusura socket in input
		
		try {
			esito = inSock.readUTF();
			System.out.println("Esito trasmissione dal server: " + esito);
			// chiudo la socket in downstream
			socket.shutdownInput();
			System.out.println("Terminata la chiusura della socket: " + socket);
		} catch (SocketTimeoutException ste) {
			System.out.println("Timeout scattato: ");
			ste.printStackTrace();
			socket.close();
			System.exit(1); //se in un ciclo continue;
		} catch (Exception e) {
			System.out.println("Error: ");
			e.printStackTrace();
			socket.close();
			System.exit(1); //se in un ciclo continue;
		}
	}
}
