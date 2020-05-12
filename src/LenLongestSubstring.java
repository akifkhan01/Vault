import java.util.*;

class LenLongestSubstring {

    public int lengthOfLongestSubstring(String s) {
        int n = s.length(), l = 0, i, j;
        
        Map<Character, Integer> map = new HashMap<>();
        
        for(i = 0, j = 0; j < n; j++) {
            
            if(map.containsKey(s.charAt(j))) {
                i = Math.max(map.get(s.charAt(j)), i);
            }
            if((j-i+1) > l) {
                l = (j-i+1);   
            }
            map.put(s.charAt(j), j+1);
        }
        
        return l;
    }

    public static void main(String args[]) {
        LenLongestSubstring a = new LenLongestSubstring();

        System.out.println(a.lengthOfLongestSubstring("abbbbbbbbbbbcdesbbbbd"));
    }
}