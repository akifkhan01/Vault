class MaximumSumSubArray {

	int computeMaximumSumSubArray(int[] arr) {

		int localMax, globalMax;
		localMax = 0;
		globalMax = Integer.MIN_VALUE;

		for(int i = 0; i < arr.length; i++) {
			localMax = Math.max(arr[i], localMax + arr[i]);
			if(localMax > globalMax)
				globalMax = localMax;
		}
		return globalMax;
	}

	public static void main(String args[]) {

		MaximumSumSubArray m = new MaximumSumSubArray();

		int[] arr = {1,2,3,4,-6,20};
		System.out.println(m.computeMaximumSumSubArray(arr));
	}
}