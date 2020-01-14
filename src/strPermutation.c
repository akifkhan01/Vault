#include<stdio.h>
#include<string.h>
#include<stdbool.h>
bool permutation(char *s1, char *s2)
{
	if(strlen(s1)!=strlen(s2))
		return false;
	int letters[128]={0},i;
	
	for(i=0;i<strlen(s1);i++)
		letters[s1[i]]++;
	
	for(i=0;i<strlen(s2);i++) {
		letters[s2[i]]--;
		if(letters[s2[i]]<0)
			return false;
	}
	return true;
}
main()
{	printf(" %s", permutation("floor","lofor") ? "true":"false");
	return 0;
}
