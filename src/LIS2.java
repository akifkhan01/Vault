import java.util.*;

class LIS2 {
	
	public int getLIS(int[] arr) throws Exception {

		if(arr == null)
			throw new Exception("Empty/null array input");

		int[] lis = new int[arr.length];

		Arrays.fill(lis, 1);

		for(int i = 1; i < arr.length; i++) 
			for(int j = 0; j < i; j++) 
				if(arr[j] < arr[i]) 
					lis[i] = Math.max(lis[i], lis[j] + 1);
		
		int max = 0;
		for(int i = 0; i < lis.length; i++) {
			System.out.print(" " + lis[i]);
			if(max < lis[i])
				max = lis[i];
		}
		System.out.println();
		return max;
	}

	public static void main(String args[]) throws Exception {

		int[] arr = {10,22,9,33,21,50,41,60,80};
		LIS2 lis = new LIS2();

		System.out.println("LIS : " + lis.getLIS(arr));
	}
}