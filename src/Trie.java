import java.util.HashMap;

class TrieNode {
	char c;
	int count;
	HashMap<Character, TrieNode> children = new HashMap<Character, TrieNode>();
	boolean terminating;

	public TrieNode() {}

	public TrieNode(char c) {
		this.c = c;
		count = 0;
	}
}

public class Trie {
	private TrieNode root;

	public Trie() {
		root = new TrieNode();
	}

	public void insert(String word) {
		HashMap<Character, TrieNode> children = root.children;

		for(int i = 0; i < word.length(); i++) {
			char c = word.charAt(i);

			TrieNode t;

			if(children.containsKey(c)) {
				t = children.get(c);
			} else {
				t = new TrieNode(c);
				children.put(c, t);
			}

			children = t.children;

			if(i == word.length()-1) {
				t.terminating = true;
				t.count++;
			}
		}
	}

	public int search(String word) {

		TrieNode t = searchNode(word);

		if(t != null && t.terminating)
			return t.count;
		
		return 0;

	}

	public boolean startsWith(String prefix) {

		if(searchNode(prefix) == null)
			return false;
		
		return true;
	}

	public TrieNode searchNode(String word) {

		HashMap<Character, TrieNode> children = root.children;
		TrieNode t = null;

		for(int i = 0; i < word.length(); i++) {
			char c = word.charAt(i);
			if(children.containsKey(c)) {
				t = children.get(c);
				children = t.children;
			} else {
				return null;
			}
		}
		return t;
	}

	public static void main(String args[]) {

		Trie trie = new Trie();

		trie.insert("ana");
		trie.insert("and");
		trie.insert("ann");
		trie.insert("bag");
		trie.insert("cat");
		trie.insert("day");
		trie.insert("dog");
		
		System.out.println("Searching and..." + trie.search("and"));
		System.out.println("Searching bags..." + trie.search("bags"));
		System.out.println("Searching startsWith da ..." + trie.startsWith("da"));
		
	}

}