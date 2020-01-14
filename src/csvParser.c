#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define HASHSIZE 10000000 

int lines=0,words=0,pureWords=0,pureNumbers=0,onlyWordsLines=0,
    onlyNumberLines=0,rowsMatchWord=0;
	
struct hashNode {
	char *value;
	struct hashNode *next;
	int count;
};	

struct hashMap {
	struct hashNode **hashTable;
	int uniqueSize,capacity;
};

struct hashMap* newHashMap() {
	struct hashMap* HashMap= (struct hashMap*)malloc(sizeof(struct hashMap));
	HashMap->hashTable=malloc(sizeof(struct hashNode *)*HASHSIZE);
	int i;
	for(i=0;i<HASHSIZE;i++)
		HashMap->hashTable[i]=NULL;
	HashMap->capacity=HASHSIZE;
	HashMap->uniqueSize=0;
	return HashMap;
}

int hashFunc(char *val) {
	int i;
	unsigned long long int hash=0;
	for(i=0;i<strlen(val);i++) {
		//31 is a Prime number used to generate Hash value
		hash=hash*31+val[i];
	}
	return hash%HASHSIZE;
}

//Insert in HashTable
int insert(struct hashMap *HashMap, char *val) {
	if(val==NULL || HashMap==NULL)
		return 0;
	int hash=hashFunc(val);
	//No Collision
	if(HashMap->hashTable[hash]==NULL) {
		struct hashNode *node = (struct hashNode *)malloc(sizeof(struct hashNode));
		node->count=1;
		node->next=NULL;
		node->value=val;
		HashMap->hashTable[hash]=node;
		HashMap->uniqueSize++;
	}
	//Collision
	else if(HashMap->hashTable[hash]!=NULL) {
		int valExists=0;
		//Traverse through LinkedList of hashNodes to check whether value exists
		struct hashNode *iter=HashMap->hashTable[hash];
		while(iter!=NULL) {
			if(strcmp(iter->value,val)==0) {
				iter->count++;
				valExists=1;
				break;
			}
			iter=iter->next;
		}
		if(valExists==0) {
			struct hashNode *node = (struct hashNode *)malloc(sizeof(struct hashNode));
			node->count=1;
			node->next=HashMap->hashTable[hash];
			node->value=val;
			//Add new node to beginning of hashTable[hash]
			HashMap->hashTable[hash]=node;
			HashMap->uniqueSize++;	
		}
	}
	return 1;
}

int checkPureWord(char *token) {
	int i,isPure=1;
	for(i=0;i<strlen(token);i++) {
		if(!((token[i]>='a' && token[i]<='z')||
			(token[i]>='A' && token[i]<='Z'))) {
			isPure=0;
			break;		
			}
	}
	if(isPure==1) {
		pureWords++;
		return 1;
	}
	return 0;
}

int checkPureNumber(char *token) {
	int i,isPure=1,decimalCount=0;
	for(i=0;i<strlen(token);i++) {
		if(token[i]=='.')
			decimalCount++;
	}
	if(decimalCount>1){
		isPure=0;
	}
	else {
		for(i=0;i<strlen(token);i++) {
			if(!(isdigit(token[i]) || token[i]=='.')) {
				isPure=0;
				break;		
			}
		}
	}
	if(isPure==1) {
		pureNumbers++;
		return 1;
	}
	return 0;
}

/*
Return 1: PureWord
	   2: PureNumber
	   3: AlphaNumeric
*/
int forEachWord(char *token, struct hashMap *HashMap) {
	words++;
	insert(HashMap,token);
	if(checkPureWord(token)) {
		return 1;
	}
	else if(checkPureNumber(token)) {
		return 2;
	}
	else {
		return 3;
	}
}

void forEachLine(char *eachLine, struct hashMap *HashMap, int col, 
				char *word) {
	lines++;
	int onlyWords=1,onlyNumbers=1,i,j;
	char *token=strtok(eachLine,",");
	j=1;
	while(token!=NULL) {
		if(j==col) {
			if(strcmp(word,token)==0) {
				rowsMatchWord++;
			}
		}
		i=forEachWord(token,HashMap);
		if(i==1)
			onlyNumbers=0;
		else if(i==2)
			onlyWords=0;
		else if(i==3) {
			onlyWords=0;
			onlyNumbers=0;
		}
        token = strtok(NULL, ",");
        j++;
    }
    if(onlyWords)
    	onlyWordsLines++;
    if(onlyNumbers)
    	onlyNumberLines++;
}

void displayStats(struct hashMap *HashMap) {
	printf("Total lines: %d\n",lines);	
	printf("Total Words: %d\n",words);	
	printf("Total Pure Words: %d\n",pureWords);	
	printf("Total Pure Numbers: %d\n",pureNumbers);	
	printf("Total Unique Words: %d\n",HashMap->uniqueSize);	
	printf("Total lines with only words: %d\n",onlyWordsLines);	
	printf("Total lines with only numbers: %d\n",onlyNumberLines);	
	printf("rows that exactly match <word> in col#: %d\n",rowsMatchWord);	
}

int main(int argc, char *argv[]) {
	int i;
	if(argc!=7) {
		printf("Invalid Input parameters\n");
		printf("Follow: Valid Input Format: ../csvParser.exe -f <filePath> -c <colNumber> -w <word>");
		return 0;
	}
	//Valid Input Format: ../csvParser.exe -f <filePath> -c <colNumber> -w <word>
	char *path=argv[2];
	int col=atoi(argv[4]);
	char *word=argv[6];
	
	struct hashMap *HashMap = newHashMap();
	FILE *fPtr=fopen(path,"r");
	char line[4000];
	if (fPtr == NULL)
        exit(EXIT_FAILURE);
    
    while (fgets(line,sizeof line,fPtr)) {
        forEachLine(line,HashMap,col,word);
    } 
    fclose(fPtr);
    displayStats(HashMap);
    return 0;
}
