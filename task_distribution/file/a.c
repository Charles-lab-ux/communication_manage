#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 5
typedef struct TreeNode{
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
}TreeNode;
TreeNode* buildTree(int *preorder,int *inorder,int preL,int preR,int inL,int inR){
    if(preL>preR)return NULL;
    TreeNode *node=(TreeNode *)malloc(sizeof(TreeNode));
    node->data=preorder[preL];
    node->left=NULL;
    node->right=NULL;
    int k;
    for(k=inL;k<=inR;k++)
        if(preorder[preL]==inorder[k])break;
    int numLeft=k-inL;
    node->left=buildTree(preorder,inorder,preL+1,preL+numLeft,inL,k-1);
    node->right=buildTree(preorder,inorder,preL+numLeft+1,preR,k+1,inR);
    return node;
} 
int searchTree(TreeNode *root, int key){
    if(root!=NULL){
        if(root->data==key)return 1;
        else if(root->data>key)return searchTree(root->left,key);
        else return searchTree(root->right,key);
    }
    return 0;
}

int main() {
    int numbers[ARRAY_SIZE] = {2,4,1,3,5};

    FILE* fwrite = fopen("atob.txt", "w");
    printf("[A] Sending random array to B: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        fprintf(fwrite, "%d\n", numbers[i]);
        printf("%d ",numbers[i]);
    }
    printf("\n");
    fclose(fwrite);

    FILE* atob = fopen("atob.ready", "w");
    fclose(atob);

    FILE* f_lock = NULL;
    while (!f_lock) {
        f_lock = fopen("btoa.ready", "r");
    }
    fclose(f_lock);

    FILE* fread = fopen("btoa.txt", "r");
    int preorder[5],inorder[5];
    printf("[A] Received preorder tree from B: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        fscanf(fread, "%d", &preorder[i]);
        printf("%d ",preorder[i]);
    }
    printf("\n[A] Received inorder tree from B: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        fscanf(fread, "%d", &inorder[i]);
        printf("%d ",inorder[i]);
    }
    printf("\n[A] Build SearchTree based preorder and inorder: ");
    TreeNode *root = buildTree(preorder,inorder,0,sizeof(preorder)/4-1,0,sizeof(inorder)/4-1);
    int key;
    printf("\n[A] Print your search key: ");
    scanf("%d",&key);
    int ans=searchTree(root,key);
    if(ans)printf("Success!\n");
    else printf("Failed!\n");

    fclose(fread);

    return 0;
}
