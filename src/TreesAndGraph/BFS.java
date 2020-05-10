package TreesAndGraph;

import java.util.*;

public class BFS {
	

	boolean pathExists(UndirectedGraph g, String a, String b) {

		// Src and dest are same
		if(a.equals(b))
			return true;

		Map<String, Boolean> visited = new HashMap<>();
		LinkedList<String> queue = new LinkedList<String>();

		for (String vertex : g.getVertices()) {
			visited.put(vertex, false);
		}

		queue.add(a);
		visited.put(a, true);

		while(queue.size() != 0) {

			String v1 = queue.poll();
			System.out.println(v1 +" ");

			for(String adjVrtx : g.getAdjacencyList(v1)) {

				if(adjVrtx.equals(b)) {
					System.out.println(adjVrtx +" ");
					return true;
				}

				if(!visited.get(adjVrtx)) {

					visited.put(adjVrtx, true);
					queue.add(adjVrtx);
				}
			}

		}
		return false;
	}

	public static void main(String args[]) {

		UndirectedGraph g = new UndirectedGraph();
		g.addVertex("a");
		g.addVertex("b");
		g.addVertex("c");
		g.addVertex("d");

		g.addEdge("a", "b");
		g.addEdge("a", "c");
		// g.addEdge("a", "d");
		g.addEdge("b", "c");
		// g.addEdge("c", "d");

		g.printGraph();

		// g.removeEdge("a", "b");
		// g.removeEdge("b", "d");
		// g.removeEdge("a", "b");
		// g.removeEdge("f", "g");
		// g.removeEdge("a", "g");

		// g.printGraph();

		BFS bfs = new BFS();

		System.out.println("Path between vertices 'b' and 'd': " + bfs.pathExists(g, "b", "d"));
		System.out.println("Path between vertices 'a' and 'b': " + bfs.pathExists(g, "a", "b"));
	}
}