import java.util.*;

class UniqueString {
	
	boolean isUnique(String s) {

		if(s.length() > 128)
			return false;

		boolean[] char_set = new boolean[128];

		for(int i = 0; i < s.length(); i++) {
			int val = s.charAt(i);

			if(char_set[val])
				return false;

			char_set[val] = true;
		}
		return true;
	}

	// Only small case strings
	boolean isUniqueWithBit(String s) {
		int checker = 0;

		for(int i = 0; i < s.length(); i++) {
			int val = s.charAt(i) - 'a';

			if((checker & (1 << val)) > 0)
				return false;

			checker |= (1 << val);
		}
		return true;
	}

	public static void main(String args[]) {

		UniqueString u = new UniqueString();

		System.out.println(u.isUniqueWithBit("akif"));
		System.out.println(u.isUnique("Akif_KHAN"));
	}
}