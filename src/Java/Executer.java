import java.util.List;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class Executer {
	private Account a;
	private List<String> info;
	private List<String> message;
	private List<String> userCommands;
	private List<String> modCommands;
	private List<String> adminCommands;
	private List<String> blockedUsers;
	private Parser p;
	private boolean enabled;

	public Executer() {
		a = new Account();
		info = new ArrayList<String>();
		message = new ArrayList<String>();
		userCommands = new ArrayList<String>(Arrays.asList("!say", "!search", "!user", "!google", "!lmgtfy", "!login", "!mods", "!blocked", "!help"));
		modCommands = new ArrayList<String>(Arrays.asList("!disable", "!enable", "!block", "!unblock", "!logout", "!modhelp"));
		adminCommands = new ArrayList<String>(Arrays.asList("!restart", "!die", "!nick", "!leave", "!join", "!channels", "!mod", "!unmod"));
		blockedUsers = new ArrayList<String>();
		
		p = new Parser();
		
		enabled = true;
	}
	
	public boolean isEnabled() {
		return enabled;
	}
	
	public void addAccount(Account a) {
		this.a = a;
	}

	public void handle(PrintWriter out, String s) {
		info = p.extractInfo(s);
		
		// needs to be a legit user message
		if (info.size() < 6 || blockedUsers.contains(info.get(0))) return;
		
		// extract each word in message section 
		message = p.extractMessage(info.get(5));
		// if !command does not start with !, not a command
		if (message.size() < 2 || message.get(1).charAt(0) != '!') return;
		
		// nickname !command parameters
		Matcher match = Pattern.compile("(\\w+).*!(\\w+) ?(.*)?").matcher(info.get(5));
		if (match.find()) {
			String params = match.group(3);
			// if !command matches any existing userCommands
			if (userCommands.contains(message.get(1).toLowerCase())) {
				System.out.println(info.get(0) + " requested command: " + message.get(1));
				userAction(out, params);
				return;
			}
			
			if (a.getAdmins().contains(info.get(0))) {
				System.out.println(info.get(0) + " requested command: " + message.get(1));
				if (adminCommands.contains(message.get(1).toLowerCase())) {
					adminAction(out, params);
					return;
				}
			}
			
			if (a.getMods().contains(info.get(0))) {
				System.out.println(info.get(0) + " requested command: " + message.get(1));
				if (modCommands.contains(message.get(1).toLowerCase())) {
					modAction(out, params);
					return;
				}
			}
		}
	}
	
	private void userAction(PrintWriter out, String params) {
		String base = null;
		String s = "";
		String contact = p.isChannel(info.get(4)) ? info.get(4) : info.get(0);
		
		params = params.replaceAll("^ +\\b", "");
		params = params.replaceAll("\\b {2,}\\b", " ");
		
		switch (message.get(1)) {
			case "!search":
				if (params.isEmpty() || params.matches("^ +$")) {
					return;
				}
				
				base = "http://null-byte.wonderhowto.com/search/";
				
				params = params.replaceAll("\\b \\b", "-");
				
				out.println("PRIVMSG " + contact + " :" + base + params);
				break;
			case "!user":
				if (params.isEmpty() || params.matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					return;
				}
				
				base = "http://null-byte.wonderhowto.com/community/search/";
				
				params = params.replaceAll("\\b \\b", "-");
				
				out.println("PRIVMSG " + contact + " :" + base + params);
				break;
			case "!google":
				if (params.isEmpty() || params.matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					return;
				}
				
				// need to add in some search results
				base = "http://google.com/#q=";
				
				params = params.replaceAll("\\b \\b", "+");
				
				out.println("PRIVMSG " + contact + " :" + base + params);
				break;
			case "!lmgtfy":
				if (params.isEmpty() || params.matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					return;
				}
				
				base = "http://lmgtfy.com/?q=";
				
				params = params.replaceAll("\\b \\b", "+");
				
				out.println("PRIVMSG " + contact + " :" + base + params);
				break;
			case "!help":
				for (String str : userCommands) {
					s = s.concat(str + ", ");
				}
				
				out.println("PRIVMSG " + info.get(0) + " :Commands: " + s);
				break;
			case "!login":
				if (message.size() < 3 || message.get(2).isEmpty()) {
					out.println("PRIVMSG " + info.get(0) + " :Please provide an appropriate parameter.");
					break;
				}
				
				if (message.get(2).equals(a.getAdminPassword())) {
					if (a.getAdmins().contains(info.get(0))) {
						out.println("PRIVMSG " + info.get(0) + " :You are already logged in.");
						break;
					}
					a.addAdmin(info.get(0));
					if (a.getMods().contains(info.get(0)) == false) {
						a.addMod(info.get(0));
					}
					out.println("PRIVMSG " + info.get(0) + " :You are now logged in as an administrator.");
				} else if (message.get(2).equals(a.getModPassword())) {
					if (a.getMods().contains(info.get(0))) {
						out.println("PRIVMSG " + info.get(0) + " :You are already logged in.");
						break;
					}
					a.addMod(info.get(0));
					out.println("PRIVMSG " + info.get(0) + " :You are now logged in as a moderator.");
				}
				break;
			case "!mods":
				if (a.getMods().isEmpty()) {
					out.println("PRIVMSG " + contact + " :No mods.");
					break;
				}
				
				for (String m : a.getMods()) {
					s = s.concat(m + ", ");
				}
				
				out.println("PRIVMSG " + contact + " :Mods: " + s);
				break;
			case "!blocked":
				if (blockedUsers.isEmpty()) {
					out.println("PRIVMSG " + contact + " :No blocked users.");
					break;
				}
				
				for (String b : blockedUsers) {
					s = s.concat(b + ", ");
				}
				
				out.println("PRIVMSG " + contact + " :Blocked: " + s);
				break;
			case "!say":
				if (message.size() < 3 || message.get(2).isEmpty() || message.get(2).matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					break;
				}
				out.println("PRIVMSG " + contact + " :" + params);
				break;
		}
	}
	
	private void modAction(PrintWriter out, String params) {
		String contact = p.isChannel(info.get(4)) ? info.get(4) : info.get(0);
		
		switch (message.get(1)) {
			case "!disable":
				enabled = false;
				out.println("PRIVMSG " + contact + " :" + a.getNickname() + " is disabled.");
				break;
			case "!enable":
				enabled = true;
				out.println("PRIVMSG " + contact + " :" + a.getNickname() + " is enabled.");
				break;
			case "!block":
				if (message.size() < 3 || message.get(2).isEmpty() || message.get(2).matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					break;
				} else if (a.getMods().contains(message.get(2)) || a.getAdmins().contains(message.get(2))) {
					out.println("PRIVMSG " + contact + " :Block denied: " + message.get(2) + " is a moderator.");
					break;
				}
				
				blockedUsers.add(message.get(2));
				out.println("PRIVMSG " + contact + " :Blocked " + message.get(2));
				break;
			case "!unblock":
				if (message.size() < 3 || message.get(2).isEmpty() || message.get(2).matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					break;
				}
				
				if (blockedUsers.contains(message.get(2))) {
					blockedUsers.remove(message.get(2));
					out.println("PRIVMSG " + contact + " :Unblocked " + message.get(2));
				} else {
					out.println("PRIVMSG " + contact + " :User is not in block list.");
				}
				break;
			case "!logout":
				if (a.getMods().contains(info.get(0))) {
					a.getMods().remove(info.get(0));
				}
				
				if (a.getAdmins().contains(info.get(0))) {
					a.getAdmins().remove(info.get(0));
				}
				
				out.println("PRIVMSG " + info.get(0) + " :You are now logged out.");
				break;
			case "!modhelp":
				String s = "";
				for (String str : modCommands) {
					s = s.concat(str + ", ");
				}
				
				out.println("PRIVMSG " + info.get(0) + " :Moderator commands: " + s);
				break;
		}
		
	}
		
	private void adminAction(PrintWriter out, String params) {
		String contact = p.isChannel(info.get(4)) ? info.get(4) : info.get(0);
		
		switch (message.get(1)) {
			case "!die":
				System.exit(0);
				break;
			case "!restart":
				//
				break;
			case "!nick":
				if (message.size() < 3 || message.get(2).isEmpty()) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					break;
				}
				
				if (message.get(2).length() < 4) {
					out.println("PRIVMSG " + contact + " :Nick must be greater than 3 characters.");
					break;
				}
				
				a.setNickname(message.get(2));
				out.println("NICK " + a.getNickname());
				break;
			case "!leave":
				if (p.isChannel(info.get(4))) {
					out.println("PART " + info.get(4));
				} else if (message.size() >= 3 && message.get(2).charAt(0) == '#') {
					out.println("PART " + message.get(2));
					a.getChannels().remove(message.get(2));
					out.println("PRIVMSG " + contact + " :Left " + message.get(2));
				} else {
					out.println("PRIVMSG " + info.get(0) + " :No channel to leave.");
				}
				break;
			case "!join":
				if (message.size() < 3 || message.get(2).charAt(0) != '#') {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate channel name.");
					break;
				}
				
				a.addChannel(message.get(2));
				out.println("JOIN " + message.get(2));
				
				if (message.get(2).equals("#")) {
					out.println("PART ##unavailable");
					a.getChannels().remove(message.get(2));
					out.println("PRIVMSG " + contact + " :Please provide an appropriate channel name.");
					break;
				}
				
				out.println("PRIVMSG " + contact + " :Joined " + message.get(2));
				break;
			case "!channels":
				String chans = "";
				if (a.getChannels().isEmpty()) {
					out.println("PRIVMSG " + contact + " :No channels.");
					break;
				}
				
				for (String s : a.getChannels()) {
					chans = chans.concat(s + ", ");
				}
				
				out.println("PRIVMSG " + contact + " :Channels: " + chans);
				break;
			case "!mod":
				if (message.size() < 3 || message.get(2).isEmpty() || message.get(2).matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					break;
				}
				
				a.addMod(message.get(2));
				out.println("PRIVMSG " + contact + " :" + message.get(2) + " is now a moderator.");
				break;
			case "!unmod":
				if (message.size() < 3 || message.get(2).isEmpty() || message.get(2).matches("^ +$")) {
					out.println("PRIVMSG " + contact + " :Please provide an appropriate parameter.");
					break;
				}
				
				a.getMods().remove(message.get(2));
				out.println("PRIVMSG " + contact + " :" + message.get(2) + " has been demoted.");
				break;
		}
	}
	
}
