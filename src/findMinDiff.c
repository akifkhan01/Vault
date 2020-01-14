#include<stdio.h>
int findMinDiff(int arr[], int m,  int n) {
	if (m==0 || n==0)
		return 0;
	int i,j,k, min_diff=9999,first,last;
	
	//Sort the array
	for(i=n;i>0;i--) {
		for(j=1;j<i;j++) {
			if(arr[j]<arr[j-1]) {
				k=arr[j];
				arr[j]=arr[j-1];
				arr[j-1]=k;
			}
		}
	}
	
	if(n<m)
		return -1;
	
	first=0;last=0;
	
	for(i=0;i+m-1<n;i++) {
		int diff=arr[i+m-1]-arr[i];
		if(diff<min_diff) {
			min_diff=diff;
			first=i;
			last=i+m-1;
		}
	}
	
	return (arr[last]-arr[first]);
}
void main() {
	int arr[] = {3,4,1,9,56,7,9,12};
	printf("%d",findMinDiff(arr,5,8));
}
