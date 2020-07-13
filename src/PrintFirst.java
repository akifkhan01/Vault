import java.util.*;

public class PrintFirst {
	
	public String printFirstLetter(String s) {

		if(s == null || s.isEmpty())
			return new String("ERROR : Empty or null string");

		String words[] = s.split(" ");
		char[] firstLetters = new char[words.length];
		int i = 0;

		for(String word : words) {
			firstLetters[i] = word.charAt(0);
			i++;
		}
		return new String(firstLetters);
	}

	public static void main(String args[]) {
		PrintFirst pf = new PrintFirst();

		System.out.println("First letters are : " + pf.printFirstLetter("Facebook, Apple, Amazon, Netflix, Google."));
	}
}