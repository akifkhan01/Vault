import java.util.*;

public class MedianOfSortedArrays {
	
	public double findMedian(int[] a, int[] b) throws Exception {
		if(a == null || b == null)
			throw new IllegalArgumentException("input is null, exit(0)");

		// We want to iterate over smaller length array
		if(a.length > b.length)
			return findMedian(b, a);

		int X, Y;
		X = a.length;
		Y = b.length;
		
		int start = 0;
		int end = X;
		
		int Px, Py;
		
		while(start <= end) {
			Px = (start + end)/2;
			Py = ((X + Y + 1)/2) - Px;
			
			int maxLeftX, maxLeftY, minRightX, minRightY;

			maxLeftX  = (Px == 0) ? Integer.MIN_VALUE : a[Px-1];
			minRightX = (Px == X) ? Integer.MAX_VALUE : a[Px];

			maxLeftY  = (Py == 0) ? Integer.MIN_VALUE : b[Py-1];
			minRightY = (Py == Y) ? Integer.MAX_VALUE : b[Py];

			// found
			if(maxLeftX <= minRightY && maxLeftY <= minRightX) {
				if((X + Y) % 2 == 0)
					return ((double)Math.max(maxLeftX, maxLeftY) + Math.min(minRightX, minRightY))/2;
				else
					return Math.max(maxLeftX, maxLeftY);

			} else if(maxLeftX > minRightY) {
				end = Px - 1;
			} else {
				start = Px + 1;
			}

 		}
 		throw new IllegalArgumentException("Arrays are not sorted, exit(0)");
	}

	public static void main(String args[]) throws Exception {
		int[] a = {2,5,9,12,15,19,23,26,31,35};
		int[] b = {3,5,7,9,11,16};

		MedianOfSortedArrays ms = new MedianOfSortedArrays();

		System.out.println("Median :" + ms.findMedian(a, b));
	}
}