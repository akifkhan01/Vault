/* Dynamic program for LIS */
/* Written in text editor */
/* Referenced from GFG LIS solution */

class LIS {
  static int lis( int arr[], int n) {
    int lis[] = new int[n];
    int i, j, max = 0;
		
    /* init LIS */
    for ( i=0; i<n; i++) 
      lis[i] = 1;
    
    /* Compute optimized values in bottom up manner */
    for( i=0; i<n; i++) {
      for( j=0; j<n; j++)
	if ( arr[i] > arr[j] && lis[i] < lis[j] + 1)
	  lis[i] = lis[j] + 1;
    }				

    /* Iterate to pull out max */    
    for( i=0; i<n; i++)
      if(max < lis[i])
	max = lis[i];

    return max;
  }

  public static void main(String args[]) {
    int arr[] = { 10, 22, 9, 33, 21, 50, 41, 60};
    int n = arr.length;
    System.out.println("Length of LIS is " + lis(arr, n));
  }
}
