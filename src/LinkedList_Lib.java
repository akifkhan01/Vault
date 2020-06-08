class LinkedList_Lib {
	
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
}