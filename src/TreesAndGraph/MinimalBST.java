package TreesAndGraph;

import java.util.*;

class MinimalBST {
	
	public TreeNode createMinimalBST(int[] arr, int start, int end) {

		if(arr == null || start > end || end < 0) return null;

		int mid = (start + end)/2;

		TreeNode n = new TreeNode(arr[mid]);

		n.left  = createMinimalBST(arr, start, mid-1);
		n.right = createMinimalBST(arr, mid+1, end);

		return n;
	}

	public static void main(String args[]) {

		int[] arr = new int[]{2,4,7,9,15,19,23,30,35};
		MinimalBST m = new MinimalBST();

		TreeNode root = m.createMinimalBST(arr, 0, arr.length - 1);

		root.preOrderTraversal(root);
		System.out.println("==========================================================================");
		root.inOrderTraversal(root);
	} 
}
