import java.util.*;

class RemoveDups_LL {
	
	static void removeDups(LinkedListNode n) {
		
		if(n == null) {
			System.out.println("LinkedList is null");
			return;
		}

		HashSet<Integer> set = new HashSet<>();
		LinkedListNode prev = n;
		n = n.next;

		while(n != null) {
			if(set.contains(n.getData())) {
				prev.next = n.next;
			} else {
				set.add(n.getData());
				prev = n;
			}
			n = n.next;
		}
	}

	static LinkedListNode createLinkedList(int[] arr) {

		LinkedListNode start = new LinkedListNode(arr[0]);
		LinkedListNode prev, current;

		prev = start;
		current = null;

		for(int i = 1; i < arr.length; i++) {
			current = new LinkedListNode(arr[i]);
			prev.next = current;

			prev = current;
			current = null;
		}

		return start;
	}

	static void printLinkedList(LinkedListNode n) {

		while(n != null) {
			System.out.println(n.getData());
			n = n.next;
		}
	}

	public static void main(String args[]) {
		int[] arr = new int[]{1,2,3,4,5,6,7,8,5,6,9};
		
		LinkedListNode start = createLinkedList(arr);
		printLinkedList(start);
		System.out.println("============================================================");
		removeDups(start);
		printLinkedList(start);

	}
}