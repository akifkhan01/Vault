public class LongestCommonSubsequence {
  
  int lcs( char[] X, char[] Y, int m, int n) {
    int L[][] = new int[m+1][n+1];
    
    for ( int i=0; i<=m; i++) {
      for( int j=0; j<=n;j++) {
        if( i==0 || j==0 ) 
          L[i][j]=0;
        else if( X[i-1] == Y[j-1])
	  L[i][j] = 1 + L[i-1][j-1];
	else
	  L[i][j] = max(L[i][j-1], L[i-1][j]);  
      }
    }
    
    System.out.printf(" ");
    for( int i=0; i<n; i++)
      System.out.printf(" %c", Y[i]);
    System.out.println();    
    for ( int i=0; i<=m; i++) {
      if(i<m)
        System.out.printf("%c ", X[i]);
      else 
        System.out.printf("  ");
      for ( int j=0; j<=n; j++)
	System.out.printf("%d ", L[i][j]);
      System.out.println();
    }
    return L[m][n];
  }

  int max ( int a, int b) {
    return (a > b)? a:b;
  }

  public static void main(String[] args) {
    LongestCommonSubsequence lcs = new LongestCommonSubsequence(); 
    String s1 = args[0]; 
    String s2 = args[1]; 
  
    char[] X=s1.toCharArray(); 
    char[] Y=s2.toCharArray(); 
    int m = X.length; 
    int n = Y.length; 
  
    System.out.println("Returned value = " + lcs.lcs(X, Y, m, n)); 
  }
}
