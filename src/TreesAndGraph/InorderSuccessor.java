package TreesAndGraph;

import java.util.*;

class InorderSuccessor {

	public TreeNodeWithParent getInorderSuccessor(TreeNodeWithParent n) {
		
		if(n == null) {
			return null;
		}

		if(n.right != null) {
			return leftMostChild(n.right);
		} else {
			TreeNodeWithParent q = n;
			TreeNodeWithParent x = q.parent;

			while(x != null && x.left != q) {
				q = x;
				x = x.parent;
			}
			return x;
		}
	}

	private TreeNodeWithParent leftMostChild(TreeNodeWithParent n) {

		if(n == null)
			return null;

		while(n.left != null) {
			n = n.left;
		}
		return n;
	}

	public static void main(String args[]) {
		InorderSuccessor successor = new InorderSuccessor();

		int[] arr = new int[]{2,4,7,9,15,19,23,30,35};
		MinimalBSTWithParent m = new MinimalBSTWithParent();

		TreeNodeWithParent root = m.createMinimalBSTWithParent(arr, 0, arr.length - 1, null);

		root.preOrderTraversal(root);
		System.out.println("==========================================================================");
		root.inOrderTraversal(root);

		System.out.println("For node : " + root.data + " :: Inorder successor : " + successor.getInorderSuccessor(root).data);
	}
}