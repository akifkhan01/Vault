package TreesAndGraph;

import java.util.*;

class CheckBST {
	
	public boolean isBST(TreeNode root, Integer min, Integer max) {

		if(root == null)
			return true;

		if((min != null && root.getKey() < min) || (max != null && max < root.getKey())) {
			return false;
		}

		if(!isBST(root.left, min, root.getKey()) || !isBST(root.right, root.getKey(), max)) {
			return false;
		}

		return true;
	}

	public boolean isBSTWithInorder(TreeNode root, Integer lastPrinted) {

		if(root == null)
			return true;

		if(!isBSTWithInorder(root.left, lastPrinted))
			return false;

		if(lastPrinted != null && lastPrinted > root.getKey())
			return false;

		lastPrinted = root.getKey();

		if(!isBSTWithInorder(root.right, lastPrinted))
			return false;

		return true;

	}

	public static void main(String args[]) {

		CheckBST checkBST = new CheckBST();
		int[] arr = new int[]{2,4,7,9,15,19,30,35};
		MinimalBST m = new MinimalBST();

		TreeNode root = m.createMinimalBST(arr, 0, arr.length - 1);

		root.preOrderTraversal(root);
		System.out.println("==========================================================================");
		root.inOrderTraversal(root);
		System.out.println("==========================================================================");
		System.out.println("Check BST : " + checkBST.isBST(root, null, null));
		System.out.println("Check BST with inorder : " + checkBST.isBSTWithInorder(root, null));
	}
}