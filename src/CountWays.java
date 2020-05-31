import java.util.*;

class CountWays {
	
	int countWaysOfSteps(int n) {
		int[] memoization = new int[n+1];
		Arrays.fill(memoization, -1);
		return countWaysOfSteps(n, memoization);
	}

	int countWaysOfSteps(int n, int[] memoization) {

		if(n < 0) {
			return 0;
		} else if(n == 0) {
			return 1;
		} else if(memoization[n] > -1) {
			return memoization[n];
		} else {
			memoization[n] = countWaysOfSteps(n-1, memoization) + countWaysOfSteps(n-2, memoization);
			return memoization[n];
		}
	}

	public static void main(String args[]) {
		CountWays cw = new CountWays();

		System.out.println("Count ways of steps : " + cw.countWaysOfSteps(47));
	}
}