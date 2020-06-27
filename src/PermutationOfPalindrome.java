import java.util.*;

class PermutationOfPalindrome {

	boolean isPermutationOfPalindrome(String s) {

		int bitVector = createBitVector(s);
		return (bitVector == 0) || (checkExactlyOneBitSet(bitVector) == 0);
	}

	private int createBitVector(String s) {

		int bitVector = 0;

		for(char c : s.toCharArray()) {
			int x = getCharNumber(c);
			bitVector = toggle(bitVector, x);
		}
		return bitVector;
	}

	private int checkExactlyOneBitSet(int bitVector) {

		/* this will return 0 if exactly one bit is set in bitVector
		 * Eg : True -> 16-15 -> (010000 & 001111 = 000000)
		 *      False-> 14-13 -> (001110 & 001101 = 001100) 
		*/
		return (bitVector & (bitVector-1));
	}

	// toggle xth bit in bitVector
	private int toggle(int bitVector, int x) {

		if(x < 0)
			return bitVector;

		int mask;
		mask = 1 << x;

		//xth bit char doesn't exist
		if((bitVector & mask) == 0) {
			bitVector |= mask;
		} else {
			// xth bit char exists, toggle its presence in bitVector
			bitVector &= ~mask;
		}
		return bitVector;
	}

	private int getCharNumber(char c) {

		int a = Character.getNumericValue('a');
		int z = Character.getNumericValue('z');

		int val = Character.getNumericValue(c);
		if(a <= val && val <= z)
			return val - a;
		else 
			return -1;
	}

	public static void main(String args[]) {

		String s = args[0];

		PermutationOfPalindrome p = new PermutationOfPalindrome();

		System.out.println(p.isPermutationOfPalindrome(s));
	}
}