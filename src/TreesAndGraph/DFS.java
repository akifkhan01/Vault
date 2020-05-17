package TreesAndGraph;

import java.util.*;

public class DFS {

	Map<String, Boolean> visited;
	UndirectedGraph g;

	public DFS(UndirectedGraph g) {

		visited = new HashMap<>();
		this.g = g;

		for (String vertex : g.getVertices()) {
			visited.put(vertex, false);
		}
	}

	void traversal(String src) {

		visited.put(src, true);
		System.out.println(src + " ");

		for(String adjVrtx : g.getAdjacencyList(src)) {

			if(!visited.get(adjVrtx)) {
				traversal(adjVrtx);
			}
		}
	}

	public static void main(String args[]) {

		UndirectedGraph g = new UndirectedGraph();
		g.addVertex("a");
		g.addVertex("b");
		g.addVertex("c");
		g.addVertex("d");
		g.addVertex("e");
		g.addVertex("f");
		
		g.addEdge("a", "b");
		g.addEdge("a", "c");
		g.addEdge("b", "c");
		g.addEdge("b", "d");
		g.addEdge("d", "e");
		g.addEdge("c", "f");
		
		g.printGraph();
		
		DFS dfs = new DFS(g);
		dfs.traversal("a");
	}
}