import java.util.*;

class BitsSum {

	int countBitsSum(int n) {

		int sum = 0;
		int[] count = new int[n+1];

		for(int i = 1; i <= n; i++) {

			if((i & 1) != 0) 
				count[i] = count[i/2] + 1;
			else
				count[i] = count[i/2];			
		}

		for(int i = 1; i <= n; i++) {
			System.out.println("i = "+ i +": count[i] = " + count[i]);
			sum += count[i];
		}
		return sum;
	}

	int countBits(int n) {

		int count = 0;

		while(n > 0) {
			if((n & 1) == 1)
				count++;
			n = n/2;
		}

		return count;
	}

	public static void main(String args[]) {

		BitsSum b = new BitsSum();

		System.out.println(b.countBitsSum(50));
		System.out.println(b.countBits(47));
	}
}