class kthLastElement_LL {
	
	static int find_Kth_LastElement_LL(int k, LinkedListNode n) {

		if(n == null) return -1;

		LinkedListNode h1, h2;
		h1 = n;
		h2 = n;

		// Move h2 'k' nodes ahead
		for(int i = 0; i < k; i++) {
			if(h2 != null) 
				h2 = h2.next;
			else {
				// Invalid value of k, greater than size of LinkedList
				return -1;
			}
		}

		if(k > 0) {
			while(h2 != null) {
				h1 = h1.next;
				h2 = h2.next;
			}
		} else if(k == 0) {
			while(h1.next != null)
				h1 = h1.next;
		} else {
			return -1;
		}

		return h1.getData();
	}

	public static void main(String args[]) {

		int[] arr = new int[]{2,4,3,5,7,1,9,12};
		LinkedListNode n = LinkedList_Lib.createLinkedList(arr);
		LinkedList_Lib.printLinkedList(n);

		System.out.println("Kth last element : " + find_Kth_LastElement_LL(Integer.parseInt(args[0]), n));
	}
}
