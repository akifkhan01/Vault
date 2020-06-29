class RotateMatrix {
	
	private boolean rotateMatrix(int[][] m) {
		
		if(m.length == 0 || m.length != m[0].length)
			return false;

		int n = m.length;

		for(int layer = 0; layer < n/2; layer++) {
			int first = layer;
			int last = n - 1 - layer;

			for(int i = first; i < last; i++) {
				int offset = i - first;
				
				//save top
				int top = m[first][i];

				// left -> top
				m[first][i] = m[last-offset][first];

				// bottom -> left
				m[last-offset][first] = m[last][last-offset];

				// right -> bottom
				m[last][last-offset] = m[i][last];

				// top -> right
				m[i][last] = top;

			}
		}
		return true;
	}		

	private void printMatrix(int[][] m) {

		if(m.length == 0) return;

		for(int i = 0; i < m.length; i++) {
			for(int j = 0; j < m[i].length; j++)
				System.out.print(m[i][j] + " ");
			System.out.println();
		}
		System.out.println("=================================================================================");

	}

	public static void main(String args[]) {
		RotateMatrix rm = new RotateMatrix();

		int[][] m = new int[][]{{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15},{16,17,18,19,20},{21,22,23,24,25}};

		rm.printMatrix(m);		
		rm.rotateMatrix(m);
		rm.printMatrix(m);		
	}
}