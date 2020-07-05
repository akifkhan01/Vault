import java.util.*;

// Directed Graph BFS
class FindPathInGraphWithBFS {
	
	public boolean pathExists(DirectedGraph g, String v1, String v2) {

		if(g == null) {
			// No graph exists
			return false;
		}

		if(v1.equals(v2))
			return true;

		LinkedList<String> queue = new LinkedList<>();
		Map<String, Boolean> visited = new HashMap<>();

		for(String vertex : g.getVertices()){
			visited.put(vertex, false);
		}

		queue.add(v1);
		visited.put(v1, true);

		while(queue.size() > 0) {

			String currentVertex = queue.poll();
			System.out.println("adjVertex : " + currentVertex);

			for(String adjVertex : g.getAdjacencyList(currentVertex)) {

				if(adjVertex.equals(v2)) {
					System.out.println("adjVertex : " + adjVertex +" ");
					return true;
				}

				if(!visited.get(adjVertex)) {
					visited.put(adjVertex, true);
					queue.add(adjVertex);
				}
			}
		}
		return false;

	}

	public static void main(String args[]) {

		DirectedGraph g = new DirectedGraph();
		g.addVertex("a");
		g.addVertex("b");
		g.addVertex("c");
		g.addVertex("d");

		g.addEdge("a", "b");
		//g.addEdge("a", "c");
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

		FindPathInGraphWithBFS bfs = new FindPathInGraphWithBFS();

		System.out.println("Path between vertices 'b' and 'd': " + bfs.pathExists(g, "b", "d"));
		System.out.println("Path between vertices 'a' and 'c': " + bfs.pathExists(g, "a", "c"));
	}
}