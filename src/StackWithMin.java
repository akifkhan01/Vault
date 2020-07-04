import java.util.*;

class StackWithMin extends Stack<Integer> {
	
	Stack<Integer> sMin;

	public StackWithMin() {
		sMin = new Stack<Integer>();
	}

	public void push(int n) {
		if(n <= min())
			sMin.push(n);
		super.push(n);
	}

	public Integer pop() {
		int val = super.pop();
		if(val == min()) {
			sMin.pop();
		}
		return val;
	}

	public Integer min() {
		if(sMin.isEmpty())
			return Integer.MAX_VALUE;
		return sMin.peek();
	}

	public void printStack() {
		Iterator v = sMin.iterator();
		while(v.hasNext()) {
			System.out.print(" " + v.next());
		}
		System.out.println("\n====================================================================================");
	}

	public void printSuperStack() {
		Iterator v = super.iterator();
		while(v.hasNext()) {
			System.out.print(" " + v.next());
		}
		System.out.println("\n====================================================================================");
	}

	public static void main(String args[]) {
		StackWithMin s = new StackWithMin();

		s.push(5);
		s.push(6);
		s.push(3);
		s.push(7);
		s.push(4);
		s.push(9);
		s.push(2);
		s.push(8);
		s.push(8);
		s.push(-8);
		s.push(-18);
		s.push(40);

		s.pop();
		
		s.printStack();
		s.printSuperStack();
		System.out.println("Min in stack: " + s.min());
	}
}