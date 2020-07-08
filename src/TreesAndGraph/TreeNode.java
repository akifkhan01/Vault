package TreesAndGraph;

public class TreeNode {
	private int key;

	TreeNode left, right;

	public TreeNode(int data) {
	key = data;
	left = right = null;
	}

	int getKey() {
		return key;
	}

	void preOrderTraversal(TreeNode node) {

		if(node == null) return;
		System.out.println(node.getKey() + " ");
		this.preOrderTraversal(node.left);
		this.preOrderTraversal(node.right);
	}

	void inOrderTraversal(TreeNode node) {

		if(node == null) return;
		this.inOrderTraversal(node.left);
		System.out.println(node.getKey() + " ");
		this.inOrderTraversal(node.right);
	}

	void postOrderTraversal(TreeNode node) {

		if(node == null) return;
		this.postOrderTraversal(node.left);
		this.postOrderTraversal(node.right);
		System.out.println(node.getKey() + " ");
	}
}