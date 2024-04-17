#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 5
typedef struct TreeNode{
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
}TreeNode;
TreeNode* insert(TreeNode *root,int data){
    if(root==NULL){
        TreeNode *newNode=(TreeNode *)malloc(sizeof(TreeNode));
        newNode->data=data;
        newNode->left=NULL;
        newNode->right=NULL;
        return newNode;
    }
    if(data<root->data)root->left=insert(root->left,data);
    else root->right=insert(root->right,data);
    return root;
}
void inorder(TreeNode *root,FILE* fwrite){
    if(root!=NULL){
        inorder(root->left,fwrite);
        fprintf(fwrite, "%d\n", root->data);
        inorder(root->right,fwrite);
    }
}
void preorder(TreeNode *root,FILE* fwrite){
    if(root!=NULL){
        fprintf(fwrite, "%d\n", root->data);
        inorder(root->left,fwrite);
        inorder(root->right,fwrite);
    }
}
TreeNode* buildBinaryTree(int *array, int size){
    TreeNode *root=NULL;
    for(int i=0;i<size/4;i++)
        root=insert(root,array[i]);
    return root;
}
int main() {
    int numbers[ARRAY_SIZE];
    FILE* f_lock = NULL;
    while (!f_lock) {
        f_lock = fopen("atob.ready", "r");
    }
    fclose(f_lock);

    FILE* fread = fopen("atob.txt", "r");
    printf("[B] Received array from A: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        fscanf(fread, "%d", &numbers[i]);
        printf("%d ",numbers[i]);
    }
    printf("\n");
    fclose(fread);
    printf("[B] Send preorder and inorder to A!\n");
    TreeNode *root=buildBinaryTree(numbers,sizeof(numbers));
    FILE* fwrite = fopen("btoa.txt", "w");
    preorder(root,fwrite);
    inorder(root,fwrite);
    fclose(fwrite);

    FILE* f_sorted = fopen("btoa.ready", "w");
    fclose(f_sorted);

    return 0;
}
