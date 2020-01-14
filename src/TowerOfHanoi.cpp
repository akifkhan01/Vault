#include<iostream>


void Tower(int a, char from, char aux, char to) {
	if(a==1) {
		std::cout<<a<<" : "<<from<<" to "<<to<<"\n";
		return;
		}
	
	else {
		Tower(a-1, from, to, aux);
		std::cout<<a<<" : "<<from<<" to "<<to<<"\n";
		Tower(a-1, aux, from, to);
	}
}


int main() {
	int n;
	std::cin>>n;
	Tower(n, 'A', 'B', 'C');
	return 0;	

}
