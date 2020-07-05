import java.util.*;

class DirectedGraph {
	
	private Map<String,List<String>> adjList;
	int eCount, vCount;

	public DirectedGraph() {
		adjList = new HashMap<>();
		eCount = 0;
		vCount = 0;
	}

	public void addVertex(String v) {
		adjList.putIfAbsent(v, new ArrayList<>());
		vCount++;
	}

	// Directed edge v1 -> v2
	public void addEdge(String v1, String v2) {
		if(adjList.get(v1) != null && adjList.get(v2) != null) {
			adjList.get(v1).add(v2);
			eCount++;
		}
	}

	public void removeEdge(String v1, String v2) {
		List<String> eV1 = adjList.get(v1);
		List<String> eV2 = adjList.get(v2);
	}

	public int getVertexCount() {
		return vCount;
	}

	public int getEdgeCount() {
		return eCount;
	}

	public List<String> getVertices() {
		List<String> vertices = new ArrayList<>();

		for(String v : adjList.keySet()) {
			vertices.add(v);
		}
		return vertices;
	}

	List<String> getAdjacencyList(String v) {

		List<String> adjacencyList;

		adjacencyList = adjList.get(v);
		if(adjacencyList != null)
			return adjacencyList;
		else {
			 adjacencyList = new ArrayList<>();
			 return adjacencyList;
		}
	}

	public void printGraph() {
		System.out.println("====================================================================");
		for(Map.Entry<String, List<String>> entry : adjList.entrySet()) {
			System.out.println("Key: " + entry.getKey());
        	System.out.println("Value: "+ entry.getValue());
		}
		System.out.println("VertexCount: "+ vCount +", EdgeCount: "+ eCount);
    	System.out.println("Vertices List: "+ getVertices());
    	System.out.println("====================================================================");
	}
}