import java.math.BigInteger;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;


public class RMI_Server extends UnicastRemoteObject implements RMI_InterfaceFile {
	private static final long serialVersionUID = 1L;

	//costruttore
	public RMI_Server() throws RemoteException	{
		super();
	}

	//implement interface methods
	public String getMD5(String echo) throws RemoteException {

		System.out.println("Server - received a request with param: " + echo);

		String response = "";
		
		//elaborating response
		MessageDigest md;
		try {
			md = MessageDigest.getInstance("MD5");
			md.reset();
			md.update(echo.getBytes());
			byte[] digest = md.digest();
			BigInteger bigInt = new BigInteger(1,digest);
			String hashtext = bigInt.toString(16);
			while(hashtext.length() < 32 ){
				hashtext = "0"+hashtext;
			}
			response = hashtext;
			
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}

		System.out.println("Server - returning response: " + response);
		return response;
	}

	//main
	public static void main(String[] args) {
		final int REGISTRYPORT = 1099;
		String registryHost = "localhost";
		String serviceName = "RMI_service_name";

		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
		try{
			RMI_Server serverRMI = new RMI_Server();
			Naming.rebind(completeName, serverRMI);
			System.out.println("RMI service: Service \"" + serviceName + "\" registered");
		}
		catch(Exception e){
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}

}
