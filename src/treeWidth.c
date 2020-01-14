#include<stdio.h>
struct node {
	int data;
	struct node *left,*right;
};
/*Function protoypes*/
int getWidth(struct node* node, int level);
int height(struct node* node);
struct node* newNode(int data);
int getMaxWidth(struct node* node);
struct node * insert(struct node* node, int key);
void preorder(struct node * node);

int getMaxWidth(struct node* node) {
	int maxWdth=0,i,width=0;
	for(i=1;i<=height(node);i++) {
		width=getWidth(node,i);
		if(width>maxWdth)
			maxWdth=width;
	}
	return maxWdth;
}

int getWidth(struct node *node, int level) {
	if(node==NULL) return 0;
	if(level==1) return 1;
	else if (level>1)
		return getWidth(node->left,level-1)+getWidth(node->right,level-1);
}

struct node* newNode(int data) {
	struct node* newNode = (struct node *)malloc(sizeof(struct node));
	newNode->data=data;
	newNode->left=NULL;
	newNode->right=NULL;
	return newNode;
}

struct node * insert(struct node* node, int key) {
	if(node==NULL) return newNode(key);
	if(key < node->data)
		node->left=insert(node->left,key);
	else 
		node->right=insert(node->right,key);
	return node;
}

void preorder(struct node *node) {
	if(node!=NULL) {
		
		printf("%d ",node->data);
		preorder(node->left);
		preorder(node->right);
	}
}
int max(int a, int b) {
	return a>b?a:b;
}
int height(struct node *node) {
	if(node==NULL) return 0;
	else
		return 1+max(height(node->left),height(node->right));
	
}
void main() { 
	/* Let us create following BST
              50
           /     \
          30      70
         /  \    /  \
       20   40  60   80 */
	
	struct node *root=NULL;
	root=insert(root,5);
	insert(root,4);
	insert(root,3);
	insert(root,7);
	insert(root,1);
	insert(root,9);
	insert(root,6);
	printf("Height : %d \n",height(root));
	preorder(root);
	printf("MaxWidth : %d \n",getMaxWidth(root));
}
