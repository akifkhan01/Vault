class LongestPalindromeSubsequence {
  

	int LPS (String s) {
    
		int n = s.length(), i, j, len;
		int L[][] = new int[n][n]; 
		
		for(i = 0; i < n; i++) 
			L[i][i] = 1;
		       
		for(len = 2; len <= n; len++) {

			for(i = 0; i < (n-len+1); i++) {

				j = i+len-1;

				if( s.charAt(i) == s.charAt(j) && len ==2) {
					L[i][j] = 2;
				}

				else if(s.charAt(i) == s.charAt(j)) {
					L[i][j] = L[i+1][j-1] + 2;
				}

				else {
					L[i][j] = max(L[i][j-1], L[i+1][j]); 
				}

			}
		}

		for(i = 0; i < n; i++) {
			for(j = 0; j < n; j++)
				System.out.print(L[i][j] + " ");
			System.out.println();
		}
		return L[0][n-1];
  	}

  	int max(int a, int b) {
  		return (a > b) ? a:b;
  	} 

  	public static void main(String args[]) {
  		LongestPalindromeSubsequence lps = new LongestPalindromeSubsequence();

  		String s = args[0];

  		System.out.println("Longest Palindrome Subsequence in String " + s + " is " + lps.LPS(s));
  	}
}
