import java.util.*;

class CutRod {

	static void CutRodOptimal(int[] p, int n) {
		int[] r = new int[n+1];
		int[] s = new int[n+1];
		int q;
		r[0] = 0;

		for(int j = 1; j <= n; j++) {
			q = Integer.MIN_VALUE;

			for(int i = 1; i <= j; i++) {
				if(q < (p[i-1] + r[j-i])) {
					q = p[i-1] + r[j-i];
					s[j] = i;
				}
			}
			r[j] = q;
		}

		for(int i = 0; i <= n; i++) {
			System.out.println(r[i]);
		}

		System.out.println("===================================================================");
		for(int i = 0; i <= n; i++) {
			System.out.println(s[i]);
		}

		System.out.println("===================================================================");
		while(n > 0) {
			System.out.println(s[n]);
			n = n - s[n];
		}
	}
	
	public static void main(String args[]) {

		int n;
		int[] p;
		p = new int[] {3,5,8,9,10,17,17,20};
		n = p.length;
		CutRodOptimal(p, n);
	}
}