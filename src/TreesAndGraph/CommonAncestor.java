package TreesAndGraph;

import java.util.*;

class Result {
	public TreeNode node;
	public boolean isAncestor;

	public Result(TreeNode n, boolean isAnc) {
		node = n;
		isAncestor = isAnc;
	}
}

class CommonAncestor {

	int CommonAncestorOfNodes(TreeNode root, TreeNode p, TreeNode q) {

		Result r = commonAncestorHelper(root, p, q);

		if(r.isAncestor) {
			return r.node.getKey();
		}
		return -1;
	}

	Result commonAncestorHelper(TreeNode root, TreeNode p, TreeNode q) {

		if(root == null) return new Result(null, false);

		if(root.getKey() == p.getKey() && root.getKey() == q.getKey()) {
			return new Result(root, true);
		}

		Result rl = commonAncestorHelper(root.left, p, q);
		if(rl.isAncestor) {
			return rl;
		}

		Result rr = commonAncestorHelper(root.right, p, q);
		if(rr.isAncestor) {
			return rr;
		}

		if(rl.node != null && rr.node != null) {
			return new Result(root, true);
		} else if(root.getKey() == p.getKey() || root.getKey() == q.getKey()) {
			boolean isAncestor = rl.node != null || rr.node != null;
			return new Result(root, isAncestor);
		} else {
			return new Result(rl.node != null ? rl.node : rr.node, false);
		}
	}


	public static void main(String args[]) {
	/*				  10
	*			   /      \
	*			  5         15
	*            / \      /     \
	*			2   7    12      18
	*		   /\   /\   /  \     / \
	*		  1  3  6 8  11  13  16  19
	*		      \    \      \   \    \
	*		   	   4    9      14  17   20
	*/			

	MinimalBSTFromSortedArray bst = new MinimalBSTFromSortedArray();
		
	int arr[] = new int[]{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

	TreeNode root = bst.createMinimalBST(arr, 0, 19);

	// Preorder traversal
	System.out.println("PreOrder");
	root.preOrderTraversal(root);

	System.out.println("====================================================================================");

	// Inorder traversal
	System.out.println("Inorder");
	root.inOrderTraversal(root);

	System.out.println("====================================================================================");

	CommonAncestor ca = new CommonAncestor();
	
	System.out.println("Common Ancestor : " + ca.CommonAncestorOfNodes(root, new TreeNode(11), new TreeNode(18)));

	}
}