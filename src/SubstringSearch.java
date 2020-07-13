import java.util.*;

class SubstringSearch {
	
	private int[] computeTemporaryArray(char[] pattern) {
		
		int[] lps = new int[pattern.length];
		int i = 1,j = 0;

		while(i < pattern.length) {
			if(pattern[i] == pattern[j]) {
				lps[i] = j+1;
				i++;
				j++;
			} else {
				if(j != 0) {
					j = lps[j-1];
				} else {
					lps[i] = 0;
					i++;
				}
			}
		}
		return lps;
	}

	public boolean KMP(char[] text, char[] pattern) {

		// Actually throw Exception
		if( text == null || pattern == null)
			return false;

		if(text.length == 0 || text.length < pattern.length) 
			return false;

		if(pattern.length == 0)
			return true;

		int[] lps = computeTemporaryArray(pattern);

		System.out.println(" :: LPS Array ::");
		for(int x = 0; x < lps.length; x++) 
			System.out.print(lps[x] + " ");

		int i = 0,j = 0;

		while(i < text.length && j < pattern.length) {

			if(text[i] == pattern[j]) {
				i++;
				j++;
			} else {
				if(j != 0) {
					j = lps[j-1];
				} else {
					i++;
				}
			}
		}
		if(j == pattern.length)
			return true;
		return false;
	}

	public static void main(String args[]) {
		String text = "pqrstdheodneodabcdabereabcdabctqodjabcdabctyedccehocheofcheruiclwbcouecovcbeoucowq";
		String pattern = "abcdabcty";

		SubstringSearch ss = new SubstringSearch();

		System.out.println("Substring search : " + ss.KMP(text.toCharArray(), pattern.toCharArray())); 
	}
}