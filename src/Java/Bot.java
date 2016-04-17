import java.io.BufferedReader;
import java.net.Socket;
import java.net.UnknownHostException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;

public class Bot {
	private Account a;
	private Executer ex;
	private Socket s;
	private final String host = "irc.freenode.net";
	private final int port = 6665;
	
	public Bot() {
		// TODO Auto-generated constructor stub
		a = new Account();
		ex = new Executer();
		ex.addAccount(a);
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		Bot bot = new Bot();
		
		bot.start(args);
	}
	
	public void start(String[] args) {
		a.parseArgs(args);
		
		System.out.println("Connecting to: " + host + ":" + port);
		
		while (true) {
			try {
				s = new Socket(InetAddress.getByName(host), port);
				BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
				PrintWriter out = new PrintWriter(s.getOutputStream(), true);
				initialise(out);
				receiver(out, in);
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
		}
	}
	
	public void initialise(PrintWriter out) {
		System.out.println("Using nick: " + a.getNickname() + "; user: " + a.getUsername());
		
		out.println("NICK " + a.getNickname());
		out.println("USER " + a.getUsername() + " 0 * :dtm");
		
		if (a.getPassword() != null) {
			out.println("PRIVMSG NickServ :IDENTIFY " + a.getPassword());
			return;
		}
		
		out.println("JOIN " + a.getChannels().get(0));
	}
	
	public void receiver(PrintWriter out, BufferedReader in) throws IOException {
		String output = null;
		boolean authed = a.getPassword() == null ? true : false;
		
		while (true) {
			if ((output = in.readLine()) != null) {
					output.trim();
				if (output.contains("PING")) {
					out.println("PONG " + output.substring(5));
				} else {
					System.out.println(output);
					if (authed == true) {
						if (output.toLowerCase().contains(a.getNickname().toLowerCase())) {
							if (ex.isEnabled() || output.contains("!enable") || output.contains("!login")) {
								ex.handle(out, output);
							}
						}
					} else if (output.contains("You are now identified")) {
						authed = true;
						out.println("JOIN " + a.getChannels().get(0));
					}
				}
			}
		}
	}
}
