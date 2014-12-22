//Giacomo Cavicchioli
//mat. 0000654859
//template: UDP_Unicast_Client.java

import java.io.*;
import java.net.*;

public class UDP_Unicast_Client {

	public static void main(String[] args) {
		
		//dichiarazione variabili di default
		int defaultTimeout = 30;
		
		// dichiarazione endopoint di comunicazione 
		InetAddress addr = null;
		int port = -1;

		// controllo argomenti di input
		try {
			if (args.length == 2) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
			} else {
				System.out.println("Usage: java UDP_Unicast_Client serverIP serverPort");
				System.exit(1);
			}
		} catch (UnknownHostException e) {
			System.out.println("Problemi nella determinazione dell'endpoint del server : ");
			e.printStackTrace();
			System.out.println("UDP_Unicast_Client: interrompo...");
			System.exit(2);
		}

		// dichiarazione socket e strutture dati di supporto
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];

		// creazione della socket datagram, settaggio timeout e creazione datagram packet
		try {
			socket = new DatagramSocket();
			socket.setSoTimeout(defaultTimeout); //TODO da settare opportunamente
			packet = new DatagramPacket(buf, buf.length, addr, port);

		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("UDP_Unicast_Client: interrompo...");
			System.exit(1);
		}

		// preparazione standard input per l'interazione con l'utente
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, immetti il primo input: ");

		try {
			// dichiarazione oggetti per la lettura/scrittura dei dati
			// come stream di byte nel pacchetto UDP
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			byte[] data = null;

			// QUI EVENTUALE DICHIARAZIONE DI VARIABILI NECESSARIE PER LA REALIZZAZIONE
			// DELLA LOGICA DEL PROGRAMMA
			String message = null;
			
			//struttura a filtro
			while ((message=stdIn.readLine()) != null) {
				// Interazione con l'utente
				System.out.println("Ricevuto: " + message);
				try {
					// QUI LOGICA PER L'INTERAZIONE CON L'UTENTE
					// AD ESEMPIO LETTURA ALTRI INPUT
					
					
					
					
				} catch (Exception e) {
					System.out.println("Problemi nell'interazione da console: ");
					e.printStackTrace();
					System.out
					.print("\n^D(Unix)/^Z(Win)+invio per uscire, immetti il primo input: ");
					continue;
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					
				}

				// riempimento e invio del pacchetto
				try {
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					// QUI INSERIMENTO DATI DI INPUT NELLO STREAM DI OUTPUT
					// ad esempio, per la scrittura della stringa "message"         
					doStream.writeUTF(message);

					// inserimento dati nel pacchetto udp
					data = boStream.toByteArray();
					packet.setData(data);

					// invio pacchetto UDP al pari
					socket.send(packet);
				} catch (IOException e) {
					System.out.println("Problemi nell'invio della richiesta: ");
					e.printStackTrace();
					System.out
					.print("\n^D(Unix)/^Z(Win)+invio per uscire, immetti il primo input: ");
					continue;
				}

				try {
					// settaggio del buffer di ricezione
					packet.setData(buf);
					
					// ricezione pacchetto UDP dal pari
					socket.receive(packet);
					
					// sospensiva solo per i millisecondi indicati, dopodichè solleva una SocketException
					
				} catch (IOException e) {
					System.out.println("Problemi nella ricezione del datagramma: ");
					e.printStackTrace();
					System.out
					.print("\n^D(Unix)/^Z(Win)+invio per uscire, immetti il primo input: ");
					continue;
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
				}
				try {
					// estrazione del risultato
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);

					// QUI ESTRAZIONE RISULTATO DALLO STREAM DI INPUT
					// ad esempio, per la lettura della stringa 
					String risultato;
					risultato = diStream.readUTF();

					// QUI STAMPA RISULTATO A VIDEO
					System.out.println("Risultato: " + risultato);

				} catch (IOException e) {
					System.out.println("Problemi nella lettura della risposta: ");
					e.printStackTrace();
					System.out
					.print("\n^D(Unix)/^Z(Win)+invio per uscire, immetti il primo input: ");
					continue;
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
				}

				// tutto ok, pronto per nuova richiesta
				System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, immetti il primo input: ");
			}
		}
		
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il client termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("UDP_Unicast_Client: termino...");
		// libero le risorse occupate: socket, file, ...
		socket.close();
	}}