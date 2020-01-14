#include<stdio.h>
int kthElement(int *a, int *b, int *m, int *n, int k) {
	if(a==m)return b[k];
	if(b==n)return a[k];
	int mid1=(m-a)/2;
	int mid2=(n-b)/2;
	if(mid1+mid2 < k) {
		if(a[mid1]>b[mid2])
			return kthElement(a,b+mid2+1,m,n,k-mid2-1);
		else
			return kthElement(a+mid1+1,b,m,n,k-mid1-1);
	}
	else {
		if(a[mid1]>b[mid2]) 
			return kthElement(a,b,a+mid1,n,k);
		else
			return kthElement(a,b,m,b+mid2,k);
	}
}
void main() {
	int a[5]={2,5,7,9,12};
	int b[8]={1,5,9,23,26,28,32,34};
	int k=10;
	printf("%d ",kthElement(a,b,a+5,b+8,k));
	
}
