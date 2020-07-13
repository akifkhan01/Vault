import java.util.*;

class CutRod2 {
	
	public int cutRod(int[] price) {

		if(price == null)
			return Integer.MIN_VALUE;
		
		int[] val = new int[price.length + 1];
		int[] seq = new int[price.length + 1];
		int max;

		for(int i = 1; i <= price.length; i++) {
			max = Integer.MIN_VALUE;
			for(int j = 1; j <= i; j++) {
				if(max < price[j-1] + val[i-j]) {
					max = price[j-1] + val[i-j];
					seq[i] = j;
				}
			}
			val[i] = max;
		}

		for(int i = 0; i < val.length; i++) {
			System.out.println(val[i]);
		}

		System.out.println("===================================================================");
		for(int i = 0; i < seq.length; i++) {
			System.out.println(seq[i]);
		}

		System.out.println("===================================================================");

		int n = seq.length - 1;
		while(n > 0) {
			System.out.println(seq[n]);
			n = n - seq[n];
		}

		return val[price.length];
	}

	public static void main(String args[]) {
		int[] price = {3,5,17,17,20};

		CutRod2 cr = new CutRod2();

		System.out.println("Maximised profit : " + cr.cutRod(price));

	}
}