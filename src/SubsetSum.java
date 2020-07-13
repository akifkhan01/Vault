import java.util.*;

class SubsetSum {
	
	public boolean isSubsetSum(int[] set, int sum) {
		if(set == null)
			return false;


		int n = set.length;
		boolean[][] subset = new boolean[n+1][sum+1];

		// For sum 0
		for(int i = 0; i <= n; i++)
			subset[i][0] = true;

		// For empty set
		for(int i = 1; i <= sum; i++)
			subset[0][i] = false;

		for(int i = 1; i <= n; i++) {

			for(int j = 1; j <= sum; j++) {

				// Already subset was able to form a sum of 'j'
				if(subset[i-1][j])
					subset[i][j] = true;

				// Set value has to be smaller than sum 'j' to be included in the sum subset
				if(set[i-1] <= j)
					// Check if current set element at 'i' can be added to achieve a sum of 'j'
					subset[i][j] = subset[i][j] || subset[i - 1][j - set[i-1]];

			}
		}

		for(int i = 0; i <= n; i++) {
			for(int j =0; j <= sum; j++)
				System.out.print(subset[i][j] + " 		");
			System.out.println("\n");
		}
		return subset[n][sum];
	}

	public static void main(String args[]) {
		int[] set = {2,3,7,8,10};
		int sum = 5;

		SubsetSum ss = new SubsetSum();

		System.out.println("Subset sum is possible : " + ss.isSubsetSum(set, sum));
	}
}