#include<stdio.h>
#include<string.h>
struct node {
	int data;
	struct node* next;
};

void deleteDups(struct node* head) {
	struct node* current=head;
	while(current!=NULL) {
		struct node* runner=current;
		while(runner->next!=NULL) {
			if(runner->next->data==current->data) {
				runner->next=runner->next->next;
			}
			else {
				runner=runner->next;
			}
		}
		current=current->next;
	}
}

void createLL(struct node* head, int data) {
	struct node* runner;
	runner=head;
	while(runner->next!=NULL) {
		runner=runner->next;
	}
	runner->next=(struct node*)malloc(sizeof(struct node));
	runner->next->data=data;
	runner->next->next=NULL;
}

void print(struct node* head) {
	struct node* runner;
	runner=head;
	while(runner!=NULL) {
		printf("%d ",runner->data);
		runner=runner->next;
	}
	printf("\n");
}
void main() {
	struct node* head = (struct node*)malloc(sizeof(struct node));
	head->data=5;
	head->next=NULL;
	createLL(head,4);
	createLL(head,3);
	createLL(head,4);
	createLL(head,1); 
	print(head);	
	deleteDups(head);
	print(head);
}
