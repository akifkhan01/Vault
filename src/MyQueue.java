import java.util.*;

class MyQueue<T> {
	
	Stack<T> stackNewest, stackOldest;

	public MyQueue() {
		stackNewest = new Stack<T>();
		stackOldest = new Stack<T>();
	}

	public void add(T val) {
		stackNewest.push(val);
	}

	public int size() {
		return stackNewest.size() + stackOldest.size();
	}

	public T peek() throws Exception {
		if(stackOldest.isEmpty()) {
			shiftElements();
		}
		return stackOldest.peek();
	}

	private void shiftElements() throws Exception {
		if(stackNewest.isEmpty()) 
			throw new Exception("Empty Queue.");
		while(!stackNewest.isEmpty()) {
				stackOldest.push(stackNewest.pop());
			}
	}

	public T remove() throws Exception { 
		if(stackOldest.isEmpty()) {
			shiftElements();
		}
		return stackOldest.pop();
	}

	public static void main(String args[]) throws Exception {

		MyQueue<Integer> myQueue = new MyQueue<Integer>();

		myQueue.add(1);
		myQueue.add(2);
		myQueue.add(3);
		myQueue.add(4);
		myQueue.add(5);
		myQueue.add(6);
		myQueue.add(7); 

		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());
		
		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

		// handle empty queue case
		System.out.println("Peek :" + myQueue.peek());
		System.out.println("Remove :" + myQueue.remove());

	}
}