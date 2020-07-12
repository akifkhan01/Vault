package TreesAndGraph;

public class TreeNodeWithParent {
	int data;

	TreeNodeWithParent left, right, parent;

	public TreeNodeWithParent(int data, TreeNodeWithParent parent) {
		this.data = data;
		this.parent = parent;
		left = right = null;
	}

	void preOrderTraversal(TreeNodeWithParent node) {

		if(node == null) return;
		if(node.parent != null)
			System.out.print("Parent : " + node.parent.data);
		System.out.println(" :: Current node data : " + node.data);
		this.preOrderTraversal(node.left);
		this.preOrderTraversal(node.right);
	}

	void inOrderTraversal(TreeNodeWithParent node) {

		if(node == null) return;
		this.inOrderTraversal(node.left);
		if(node.parent != null)
			System.out.print("Parent : " + node.parent.data);
		System.out.println(" :: Current node data : " + node.data);
		this.inOrderTraversal(node.right);
	}

	void postOrderTraversal(TreeNodeWithParent node) {

		if(node == null) return;
		this.postOrderTraversal(node.left);
		this.postOrderTraversal(node.right);
		if(node.parent != null)
			System.out.print("Parent : " + node.parent.data);
		System.out.println(" :: Current node data : " + node.data);
	}
}