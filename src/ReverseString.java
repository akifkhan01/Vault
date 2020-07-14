import java.util.*;

public class ReverseString {
	
	public String reverse(String s) {

		if(s == null || s.length() < 1)
			return null;

		String[] arr = s.split(" ");

		if(arr.length < 2)
			return s;

		String r = new String();

		for(int i = arr.length - 1; i >= 0; i--) {

			if(i == 0)
				r = r + arr[i];
			else
				r = r + arr[i] + " ";
		}
		return r;
	}

	public static void main(String args[]) {

		ReverseString rs = new ReverseString();

		System.out.println("Reversed string : " + rs.reverse("I am Akif Khan"));
	}
}