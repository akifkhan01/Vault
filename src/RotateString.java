class RotateString {
	
	private boolean isRotation(String s1, String s2) {

		if(s1 == null || s2 == null) 
			return false;

		if(s1.length() != s2.length())
			return false;

		String s3 = s1 + s1;

		return isSubString(s2, s3);
	}

	// Check s1 is substring of s2
	private boolean isSubString(String s1, String s2) {
		int m = s1.length();
		int n = s2.length();

		if(m > n) return false;
		
		System.out.println("s1: " + s1 + " :: s2: " + s2);		
		for(int i = 0; i <= (n-m); i++) {

			int j;
			for(j = 0; j < m; j++) {

				System.out.println("s1 char: " + s1.charAt(j) + " :: s2 char: " + s2.charAt(i+j));
				if(s1.charAt(j) != s2.charAt(i+j))
					break;
			}
			// It is a substring
			if(j == m)
				return true;
		}

		return false;
	}

	public static void main(String args[]) {

		RotateString rs = new RotateString();

		System.out.println(rs.isRotation(args[0],args[1]));
	}
}