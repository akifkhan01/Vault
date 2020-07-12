package TreesAndGraph;

import java.util.*;

class BalancedTree {
	 

	private int checkHeight(TreeNode root) {
		if(root == null)
			return -1;

		int leftHeight = checkHeight(root.left);
		if(leftHeight == Integer.MIN_VALUE)
			return Integer.MIN_VALUE;

		int rightHeight = checkHeight(root.right);
		if(rightHeight == Integer.MIN_VALUE)
			return Integer.MIN_VALUE;

		int heightDiff = leftHeight - rightHeight;

		if(Math.abs(heightDiff) > 1) {
			return Integer.MIN_VALUE;
		} else {
			return Math.max(leftHeight, rightHeight) + 1;
		}		


	}

	public boolean checkBalancedTree(TreeNode root) {

		return checkHeight(root) != Integer.MIN_VALUE;
	}

	public static void main(String args[]) {

		/*					 5
		 *				   /  \
		 *				  4     3
		 *				 / \   /  \
		 *				10  2  9   13
		 *			   / \         / \
		 *			  20  12      15  25
		 *           / 				
		 *			51					
		*/
		TreeNode root = new TreeNode(5);
		TreeNode n1 = new TreeNode(4);
		TreeNode n2 = new TreeNode(3);
		TreeNode n3 = new TreeNode(10);
		TreeNode n4 = new TreeNode(2);
		TreeNode n5 = new TreeNode(9);
		TreeNode n6 = new TreeNode(13);
		TreeNode n7 = new TreeNode(20);
		TreeNode n8 = new TreeNode(12);
		TreeNode n9 = new TreeNode(15);
		TreeNode n10 = new TreeNode(25);
		TreeNode n11 = new TreeNode(51);

		root.left = n1;
		root.right = n2;
		
		n1.left = n3;
		n1.right = n4;

		n2.left = n5;
		n2.right = n6;

		n3.left = n7;
		n3.right = n8;

		n6.left = n9;
		n6.right = n10;

		n7.left = n11;

		BalancedTree bt = new BalancedTree();

		System.out.println("Is balanced : " + bt.checkBalancedTree(root));
	}
}