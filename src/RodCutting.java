class RodCutting {
	
	static int cutRod(int price[], int n) {
		int val[] = new int[n+1], maxVal;

		val[0] = 0;

		for(int i = 1; i <= n; i++) {
			
			maxVal = Integer.MIN_VALUE;

			for(int j = 0; j < i; j++)
				maxVal = Math.max(maxVal, price[j] + val[i-j-1]);
			
			val[i] = maxVal;
		}

	for(int i = 0; i <=n; i++)
		System.out.print(val[i]+" ");

	return val[n];
	}

	public static void main(String args[]) {

		int price[] = new int[]{5, 15, 12, 10, 19, 18, 8};

		System.out.println("Optimal price is " + cutRod(price, price.length));
	}
}