import java.util.*;

class SortStack {
	
	public void sortStack(Stack<Integer> s1) {
		Stack<Integer> s2 = new Stack<Integer>();
		int temp;
		while(!s1.isEmpty()) {
			temp = s1.pop();
			while(!s2.isEmpty() && temp < s2.peek()) {
				s1.push(s2.pop());
			}
			s2.push(temp);
		}
		while(!s2.isEmpty())
			s1.push(s2.pop());
	}

	public void printStack(Stack<Integer> s) {
		Iterator v = s.iterator();
		while(v.hasNext()) {
			System.out.print(" " + v.next());
		}
		System.out.println("\n====================================================================================");
	}

	public static void main(String args[]) {

		SortStack ss = new SortStack();

		Stack<Integer> s = new Stack<Integer>();

		s.push(3);
		s.push(1);
		s.push(2);
		s.push(4);
		s.push(9);
		s.push(5);
		s.push(20);
		s.push(12);
		s.push(14);
		s.push(11);

		ss.printStack(s);
		ss.sortStack(s);
		ss.printStack(s);
		
	}
}