#include<stdio.h>
void kWindow(int *a, int n, int k) {
	int i,j,max;
	for(i=0;i<=n-k;i++) {
		max=a[i];
		for(j=1;j<k;j++) {
			if(a[j+i]>max)
				max=a[j+i];
		}
		printf("%d ",max);
	}
}
void main() {
	int a[]={8, 5, 10, 7, 9, 4, 15, 12, 90, 13};
	int n = sizeof(a)/sizeof(a[0]);
	int k=4;
	kWindow(a,n,k);
}
