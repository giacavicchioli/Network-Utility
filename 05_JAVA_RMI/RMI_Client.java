import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.rmi.Naming;


public class RMI_Client {

	// Avvio del Client RMI
	public static void main(String[] args) {
		final int REGISTRYPORT = 1099;
		String registryHost = null;
		String serviceName = "RMI_service_name";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo dei parametri della riga di comando
		if(args.length != 1){
			System.out.println("Using: RMI_Client serverAddress");
			System.exit(1);
		}
		registryHost = args[0];

		// Connessione al servizio RMI remoto
		try{
			String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
			RMI_InterfaceFile serverRMI = (RMI_InterfaceFile) Naming.lookup(completeName);
			System.out.println("ClientRMI: Service \"" + serviceName + "\" connected");

			//inizio logica programma
			System.out.println("\nType a string to send to the server: ");
			String text;
			String response;
			while ((text= stdIn.readLine()) != null){
				response = serverRMI.getMD5(text);
				
				System.out.println("Response from RMI server: " + response);				
			}
		}	catch(Exception e){
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}

}
