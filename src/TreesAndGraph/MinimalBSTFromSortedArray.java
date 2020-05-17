package TreesAndGraph;

import java.util.*;


public class MinimalBSTFromSortedArray {

	TreeNode createMinimalBST(int[] arr, int i, int j) {
		if(i > j || j < 0) return null;

		int mid = (i + j)/2;

		TreeNode node = new TreeNode(arr[mid]);

		node.left = createMinimalBST(arr, i, mid-1);
		node.right = createMinimalBST(arr, mid+1, j);

		return node;
	}

	public static void main(String args[]) {
		MinimalBSTFromSortedArray c = new MinimalBSTFromSortedArray();
		
		int arr[] = new int[]{3,6,9,12,15,18,21,24,27,30};

		TreeNode root = c.createMinimalBST(arr, 0, 9);

		root.preOrderTraversal(root);
		
	}
}