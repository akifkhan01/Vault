import java.util.*;

class LCS {
	
	public int findLCS(char[] x, char[] y) {

		int[][] lcs = new int[x.length + 1][y.length + 1];

		for(int i = 0; i <= x.length; i++) {

			for(int j = 0; j <= y.length; j++) {

				if(i == 0 || j == 0)
					lcs[i][j] = 0;
				else if(x[i-1] == y[j-1])
					lcs[i][j] = lcs[i-1][j-1] + 1;
				else
					lcs[i][j] = Math.max(lcs[i-1][j], lcs[i][j-1]);
			}
		}
		return lcs[x.length][y.length];
	}

	public static void main(String args[]) {
		LCS lcs = new LCS(); 
    	String s1 = "AGGTAB"; 
    	String s2 = "GXTXAYB"; 
  
    	char[] X=s1.toCharArray(); 
    	char[] Y=s2.toCharArray(); 
    
    	System.out.println("Length of LCS is " + lcs.findLCS( X, Y)); 
	}
}