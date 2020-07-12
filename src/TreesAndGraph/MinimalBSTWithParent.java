package TreesAndGraph;

import java.util.*;

class MinimalBSTWithParent {
	
	public TreeNodeWithParent createMinimalBSTWithParent(int[] arr, int start, int end, TreeNodeWithParent parent) {

		if(arr == null || start > end || end < 0) return null;

		int mid = (start + end)/2;

		TreeNodeWithParent n = new TreeNodeWithParent(arr[mid], null);
		n.parent = parent;

		n.left  = createMinimalBSTWithParent(arr, start, mid-1, n);
		n.right = createMinimalBSTWithParent(arr, mid+1, end, n);

		return n;
	}
}