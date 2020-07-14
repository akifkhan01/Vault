import java.util.*;

// Using max-heap
class HeapSort {

	// heapify subtree rooted at i, heap size is 'n'
	private void heapify(int[] A, int n, int i) {
		
		int leftChild, rightChild, largest;

		largest = i;
		leftChild = 2*i + 1;
		rightChild = 2*i + 2;

		if(leftChild < n && A[leftChild] > A[largest])
			largest = leftChild;

		if(rightChild < n && A[rightChild] > A[largest])
			largest = rightChild;

		// Larger element found amongst children of A[i], swap larger element to index 'i'
		if(largest != i) {
			int t = A[i];
			A[i] = A[largest];
			A[largest] = t;

			// Recursively heapify affected subtree
			heapify(A, n, largest);
		}
	}

	public void sort(int[] A) {
		if(A == null || A.length == 0) 
			return;

		int n = A.length;

		// Build heap
		for(int i = (n/2)-1; i >= 0; i--) {
			heapify(A, n, i);
		}

		//Sort the heap
		for(int i = n-1; i >= 0; i--) {

			// Extract max element from root and put at the end of the array, moving it out of the heap
			int t = A[i];
			A[i] = A[0];
			A[0] = t;

			heapify(A, i, 0);
		}
	}

	public void printArray(int[] A) {

		if(A == null)
			return;

		for(int i = 0; i < A.length; i++) {
			System.out.print(A[i] + " ");
		}
		System.out.println();
	}

	public static void main(String args[]) {
		int[] A = {23,13,45,11,9,78,95,34,62,19,35};

		HeapSort hs = new HeapSort();

		hs.sort(A);

		hs.printArray(A);
	}
 }