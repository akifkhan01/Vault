import java.util.*;

class LoopDetectionLinkedList {
	
	LinkedListNode detectLoop(LinkedListNode n) {

		LinkedListNode fast, slow;
		fast = n;
		slow = n;

		while(fast != null && fast.next != null) {

			slow = slow.next;
			fast = fast.next.next;

			if(slow == fast) {
				//got collision node
				break;
			}
		}

		if(fast == null || fast.next == null) {
			//no cycle 
			return null;
		}

		// move fast node to LinkedList head
		fast = n;

		while(slow != fast) {
			slow = slow.next;
			fast = fast.next;
		}

		return fast;
	}

	public static void main(String args[]) {

		LoopDetectionLinkedList l = new LoopDetectionLinkedList();

		LinkedListNode n1 = new LinkedListNode(1);

		LinkedListNode n2 = new LinkedListNode(2);
		n1.next = n2;

		LinkedListNode n3 = new LinkedListNode(3);
		n2.next = n3;

		LinkedListNode n4 = new LinkedListNode(4);
		n3.next = n4;

		LinkedListNode n5 = new LinkedListNode(5);
		n4.next = n5;

		LinkedListNode n6 = new LinkedListNode(6);
		n5.next = n6;

		LinkedListNode n7 = new LinkedListNode(7);
		n6.next = n7;

		LinkedListNode n8 = new LinkedListNode(8);
		n7.next = n8;

		LinkedListNode n9 = new LinkedListNode(9);
		n8.next = n9;

		LinkedListNode n10 = new LinkedListNode(10);
		n9.next = n10;
		n10.next = n4;

		LinkedListNode loopHead = l.detectLoop(n1);
		if(loopHead != null)
			System.out.println("Loop Head : " + loopHead.getData());
		else
			System.out.println("Loop Head : " + loopHead);
	}
}