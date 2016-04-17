import java.util.List;
import java.util.ArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class Parser {
	
	public Parser() {
	}
	
	public List<String> extractInfo(String s) {
		List<String> l = new ArrayList<String>();
		Matcher message = Pattern.compile(":(.*)!(.*)@(\\S+) (.*) (.*) :(.*)").matcher(s);
		if (message.find()) {
			l.add(message.group(1));
			l.add(message.group(2));
			l.add(message.group(3));
			l.add(message.group(4));
			l.add(message.group(5));
			l.add(message.group(6));
		}
		
		return l;
	}
	
	public List<String> extractMessage(String s) {
		List<String> l = new ArrayList<String>();
		
		String[] temp = s.split(" ");
		
		for (String str : temp) {
			l.add(str);
		}
		
		return l;
	}
	
	public boolean isChannel(String s) {
		return s.charAt(0) == '#' ? true : false;
	}

}
