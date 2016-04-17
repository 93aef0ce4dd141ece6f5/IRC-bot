import java.util.List;
import java.util.ArrayList;

public class Account {
	private List<String> channels;
	private List<String> admins;
	private List<String> mods;
	private String nickname;
	private String username;
	private String password;
	private final String modPassword = "<modpasswordhere>";
	private final String adminPassword = "<adminpasswordhere>";

	public Account() {
		channels = new ArrayList<String>();
		admins = new ArrayList<String>();
		mods = new ArrayList<String>();
		nickname = null;
		username = null;
		password = null;
	}
	
	public Account(String c, String n, String u, String p) {
		channels.add(c);
		nickname = n;
		username = u;
		password = p;
	}
	
	public List<String> getChannels() {
		return channels;
	}

	public void addChannel(String channel) {
		this.channels.add(channel);
	}

	public String getNickname() {
		return nickname;
	}

	public void setNickname(String nickname) {
		this.nickname = nickname;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public void setPassword(String password) {
		this.password = password;
	}
	
	public String getPassword() {
		return password;
	}
	
	public void addAdmin(String s) {
		admins.add(s);
	}
	
	public void removeAdmin(String s) {
		admins.remove(s);
	}

	public List<String> getAdmins() {
		return admins;
	}

	public String getAdminPassword() {
		return adminPassword;
	}
	
	public void addMod(String s) {
		mods.add(s);
	}
	
	public void removeMod(String s) {
		mods.remove(s);
	}
	
	public List<String> getMods() {
		return mods;
	}
	
	public String getModPassword() {
		return modPassword;
	}
	
	public void parseArgs(String[] args) {
		channels.add(args[0]);
		nickname = args[1];
		username = args[2];
		if (args.length > 3) {
			password = args[3];
		}
	}

}
