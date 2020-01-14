#include<stdio.h>
#include<stdlib.h>

struct node 
{
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

void inorder(struct node *root)
{
    if(root != NULL)
    {
        inorder(root->left);
        printf("%d\n", root->key);
        inorder(root->right);
    }
}

struct node * insert(struct node* node, int key)
{
    if(node == NULL) return newNode(key);
    
    if(key < node->key)
        node->left = insert(node->left, key);
        
    else if(key > node->key)
        node->right = insert(node->right, key);
    
    return node;
}

struct node* minValueNode(struct node* node)
{
	struct node* current = node;
	while(current->left==NULL)
	{
		current = current->left;
	}
	return current;
}

struct node* deleteNode(struct node* root, int key)
{
	if(root == NULL) return root;
	
	if(key < root->key)
		root->left = deleteNode(root->left, key);
		
	else if(key > root->key)
		root->right = deleteNode(root->right, key);
		
	else
	{
		//one child or no child
		if(root->left == NULL)
		{
			struct node *temp = root->right;
			free(root);
			return temp;
		}
		
		else if(root->right == NULL)
		{
			struct node *temp = root->left;
			free(root);
			return temp;
		}
		
		//node wth 2 children, get Inorder successor 
		//(smallest in right sub tree)
		struct node* temp = minValueNode(root->right);
		root->key = temp->key;
		
		//delete inorder successor
		root->right = deleteNode(root->right, temp->key);
	}
}

int main()
{
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
    
    printf("\nDelete 20\n");
    root = deleteNode(root, 20);
    printf("Inorder traversal of the modified tree \n");
    inorder(root);
 
    printf("\nDelete 30\n");
    root = deleteNode(root, 30);
    printf("Inorder traversal of the modified tree \n");
    inorder(root);
 
    printf("\nDelete 50\n");
    root = deleteNode(root, 50);
    printf("Inorder traversal of the modified tree \n");
    inorder(root);
  
    return 0;
}
