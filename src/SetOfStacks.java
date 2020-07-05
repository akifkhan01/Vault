import java.util.*;

class Stack2 extends Stack<Integer> {

	int capacity;

	public Stack2(int capacity) {
		this.capacity = capacity;
	}

	public boolean isFull() {
		return capacity == super.size();
	}

	public void printStack() {
		Iterator v = this.iterator();
		while(v.hasNext()) {
			System.out.print(" " + v.next());
		}
		System.out.println("\n====================================================================================");
	}
}

class SetOfStacks {
	
	ArrayList<Stack2> stacks;
	int capacityOfEachStack;

	public SetOfStacks(int capacityOfEachStack) {
		stacks = new ArrayList<Stack2>();
		this.capacityOfEachStack = capacityOfEachStack;
	}

	public void push(int n) {
		Stack2 lastStack = getLastStack();

		if(lastStack != null && !lastStack.isFull()) {
			lastStack.push(n);
		} else {
			lastStack = new Stack2(capacityOfEachStack);
			lastStack.push(n);
			stacks.add(lastStack);
		}
	}

	public Integer pop() throws Exception {
		Stack2 lastStack = getLastStack();
		if(lastStack != null && !lastStack.isEmpty()) {
			Integer n = lastStack.pop();
			System.out.println("Popped: " + n);	
			if(lastStack.isEmpty()) {
				stacks.remove(stacks.size()-1);
			}
			return n;
		}
		throw new Exception("Empty set of stacks! No more pop() element is possible.");
	}

	private Stack2 getLastStack() {
		if(stacks.size() == 0)
			return null;
		return stacks.get(stacks.size() - 1);
	}

	public void printSetOfStacks() {
		System.out.println("Set Of Stacks : ");
		for(int i = 0; i < stacks.size(); i++) {
			Stack2 currentStack = stacks.get(i);
			currentStack.printStack();
		}
		System.out.println("\n====================================================================================");
	}

	public static void main(String args[]) throws Exception {
		SetOfStacks ss = new SetOfStacks(3);

		ss.push(1);
		ss.push(2);
		ss.push(3);

		ss.push(4);
		ss.push(5);
		ss.push(6);

		ss.push(7);
		ss.push(8);
		ss.push(9);

		ss.push(10);
		ss.push(11);
		ss.push(12);
		ss.printSetOfStacks();

		ss.pop();
		ss.pop();
		ss.printSetOfStacks();

		ss.pop();
		ss.printSetOfStacks();

		ss.pop();
		ss.printSetOfStacks();

		ss.pop();
		ss.pop();
		ss.pop();
		ss.printSetOfStacks();

		ss.pop();
		ss.pop();
		ss.pop();
		ss.pop();
		ss.pop();

		// Call with Empty set of stacks
		ss.pop();
		ss.printSetOfStacks();
	}
}
