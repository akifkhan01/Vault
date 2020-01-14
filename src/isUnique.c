 #include<stdio.h>
 #include<string.h>
 #include<stdbool.h>
 bool isUniqueChars(char *str)
 {
 	int i,val;
 	if(strlen(str) > 128) return false;
 	bool char_set[128]={false};
 	for(i = 0; i < strlen(str); i++)
 		{
 			val = str[i];
 			if(char_set[val] == true)
 				{
				return false;
				}
			char_set[val] = true; 
		}
	return true;
 }
 main()
 {
 	char ip[256];
 	scanf("%s", &ip);
 	printf(" %s", isUniqueChars(ip) ? "true":"false");
 	return 0;
 }
 
