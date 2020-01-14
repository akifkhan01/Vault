#include<stdio.h>
struct node {
	int key;
	struct node *left, *right;
};

struct node * newNode(int item) 
{
    struct node *temp = (struct node *)malloc(sizeof(struct node));
    temp->key = item;
    temp->left = temp->right = NULL;
    return temp;
}

struct node * insert(struct node *node, int key) {
	if(node==NULL) return newNode(key);
	
	if(key< node->key) 
		node->left=insert(node->left,key);
	
	else if(key>= node->key) 
		node->right=insert(node->right,key);
	
	return node;	
}

void inorder(struct node *root)
{
    if(root != NULL)
    {
        inorder(root->left);
        printf("%d\n", root->key);
        inorder(root->right);
    }
}

void printLevelOrder(struct node *node) {
	int h = height(node);
	int i;
	for(i=1;i<=h;i++)
		printGivenLevel(node, i);
}

void printGivenLevel(struct node *node,int level) {
	if(node==NULL) return;
	if(level==1)
		printf("%d ", node->key);
	else if(level>1) {
		printGivenLevel(node->left, level-1);
		//printGivenLevel(node->right, level-1);
	}
		
}

int height(struct node *node) {
	if(node==NULL) return 0;
	
	else {
		int lheight=height(node->left);
		int rheight=height(node->right);
		
		if(lheight>rheight)
			return(lheight+1);
		else
			return(rheight+1);
	}
}
void main() {
	 /* Let us create following BST
              50
           /     \
          30      70
         /  \    /  \
       20   40  60   80 */
    struct node *root = NULL;
    root = insert(root, 50);
    insert(root, 30);
    insert(root, 20);
    insert(root, 40);
    insert(root, 70);
    insert(root, 60);
    insert(root, 80);
    // print inorder traversal of the BST
    inorder(root);
    printLevelOrder(root);
}
