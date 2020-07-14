import java.util.*;

public class AnagramSearch {
	
	private static final int MAX = 256;


	private boolean compare(char[] a, char[] b) {

		for(int i = 0; i < MAX; i++) {
			if(a[i] != b[i])
				return false;
		}
		return true;
	}

	public void findAnagram(String text, String pattern) {
		if(text == null || pattern == null)
			return;

		if(text.length() < pattern.length()) {
			System.out.println("search pattern length is greater than text length, exit(0)");
			return;
		}

		int N = text.length();
		int M = pattern.length();

		char[] countP = new char[MAX];
		char[] countTW = new char[MAX];

		// Mark characters in pattern and initial set of M chars in text in char arrays for comparision
		for(int i = 0; i < M; i++) {
			(countP[pattern.charAt(i)])++;
			(countTW[text.charAt(i)])++;
		}

		for(int i = M; i < N; i++) {
			
			if(compare(countP, countTW)) {
				System.out.println("Found at index :" + (i-M));
			}

			// Add new char at 'i' in text to countTW
			(countTW[text.charAt(i)])++;

			// Remove first old character from countTW
			(countTW[text.charAt(i-M)])--;
		}

		// For last pattern match at index (N-M)
		if(compare(countP, countTW)) {
			System.out.println("Found at index :" + (N-M));
		}
	}

	public static void main(String args[]) {
		String text = "BACDGABCDA";
		String pattern = "ABCD";
		
		AnagramSearch as = new AnagramSearch();
		as.findAnagram(text, pattern);
	}
}