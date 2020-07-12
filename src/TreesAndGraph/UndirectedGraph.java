package TreesAndGraph;

import java.util.*;

public class UndirectedGraph {
	private Map<String, List<String>> adjVertices; 

	int vCount, eCount;

	public UndirectedGraph() {
		adjVertices = new HashMap<>();
		vCount = 0;
		eCount = 0;
	}

	void addVertex(String name) {
		adjVertices.putIfAbsent(name, new ArrayList<>());
		vCount++;
	}

	void addEdge(String v1, String v2) {

		if(adjVertices.get(v1) != null && adjVertices.get(v2) != null) { 
			adjVertices.get(v1).add(v2);
			adjVertices.get(v2).add(v1);
			eCount++;
		}
	}

	void removeEdge(String v1, String v2) {
		
		List<String> eV1 = adjVertices.get(v1);
		List<String> eV2 = adjVertices.get(v2);

		if(eV1 != null && eV2 != null) {
			if(eV1.contains(v2) && eV2.contains(v1)) {
				eV1.remove(v2);
				eV2.remove(v1);
				eCount--;
				System.out.println("Removed edge with vertices "+ v1 + " and " + v2);	
			} else {
				System.out.println("Edge with vertices "+ v1 + " and " + v2 +" doesn't exist");	
			}
		} else {
			System.out.println("Either of the vertices "+ v1 + " and " + v2 +" doesn't exist");
		}
	}

	int getVertexCount() {
		return vCount;
	}

	int getEdgeCount() {
		return eCount;
	}

	List<String> getVertices() {

		List<String> vertices = new ArrayList<>();

		for (String key : adjVertices.keySet()) {
			vertices.add(key);
		}

		return vertices;	
	}

	List<String> getAdjacencyList(String v) {

		List<String> adjacencyList;

		adjacencyList = adjVertices.get(v);
		if(adjacencyList != null)
			return adjacencyList;
		else {
			 adjacencyList = new ArrayList<>();
			 return adjacencyList;
		}
	}	

	void printGraph() {

		System.out.println("====================================================================");
		for (Map.Entry<String, List<String>> entry : adjVertices.entrySet()) {
        	System.out.println("Key: " + entry.getKey());
        	System.out.println("Value: "+ entry.getValue());
    	}

    	System.out.println("VertexCount: "+ vCount +", EdgeCount: "+ eCount);
    	System.out.println("Vertices List: "+ getVertices());
    	System.out.println("====================================================================");
	}
}