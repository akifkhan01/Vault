package TreesAndGraph;

import java.util.*;

class LevelledLinkedList {
	
	public ArrayList<LinkedList<TreeNode>> createLevelLL(TreeNode root) {
		
		ArrayList<LinkedList<TreeNode>> result = new ArrayList<LinkedList<TreeNode>>();

		LinkedList<TreeNode> current = new LinkedList<>();

		if(root != null)
			current.add(root);

		while(current.size() > 0) {
  			result.add(current);
  			LinkedList<TreeNode> parents = current;
  			current = new LinkedList<TreeNode>();

  			for(TreeNode parent : parents) {
    			if(parent.left != null) current.add(parent.left);
    			if(parent.right != null) current.add(parent.right);
  			}
		}
		return result;
  	}

  	void printLevelLinkedList(ArrayList<LinkedList<TreeNode>> levelLinkedList) {
		
		int level = 0;

		for(LinkedList<TreeNode> levelNodes: levelLinkedList) {
			System.out.println("====================================================================================");
			System.out.println("LEVEL : " + level);
			for(TreeNode node: levelNodes)
				System.out.println("Node : " + node.getKey());				
			level++;
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

	LevelLinkedList ll = new LevelLinkedList();

	ArrayList<LinkedList<TreeNode>> levelLinkedList = ll.createLevelLinkedList(root);

	ll.printLevelLinkedList(levelLinkedList);
	
  	}
}