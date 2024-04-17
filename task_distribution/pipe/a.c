#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 100
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
void inorder(TreeNode *root,int fd){
    if(root!=NULL){
        inorder(root->left,fd);
        write(fd, &root->data, sizeof(root->data));
        inorder(root->right,fd);
    }
}
void preorder(TreeNode *root,int fd){
    if(root!=NULL){
        write(fd, &root->data, sizeof(root->data));
        inorder(root->left,fd);
        inorder(root->right,fd);
    }
}
TreeNode* buildBinaryTree(int *array, int size){
    TreeNode *root=NULL;
    for(int i=0;i<size/4;i++)
        root=insert(root,array[i]);
    return root;
}
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
    int pipe_a_to_b[2]; // A->B
    int pipe_b_to_a[2]; // B->A
    
    if (pipe(pipe_a_to_b) == -1 || pipe(pipe_b_to_a) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) { // 父进程
        close(pipe_a_to_b[0]); // 关闭A->B读
        close(pipe_b_to_a[1]); // 关闭B->A写
        
        // 向B发送内容
        int random_array[5] = {2, 4, 1, 3, 5};
        printf("[A] Sending random array to B: ");
        for (int i = 0; i < 5; ++i) {
            printf("%d ", random_array[i]);
        }
        printf("\n");
        write(pipe_a_to_b[1], random_array, sizeof(random_array));
        
        // 接受B的内容
        int preorder[5],inorder[5];
        read(pipe_b_to_a[0], preorder, sizeof(preorder));
        read(pipe_b_to_a[0], inorder , sizeof(inorder));
        printf("[A] Received preorder tree from B: ");
        for (int i = 0; i < 5; ++i) 
            printf("%d ", preorder[i]);
        printf("\n[A] Received inorder tree from B: ");
        for (int i = 0; i < 5; ++i) 
            printf("%d ", inorder[i]);
        printf("\n[A] Build SearchTree based preorder and inorder: ");
        TreeNode *root = buildTree(preorder,inorder,0,sizeof(preorder)/4-1,0,sizeof(inorder)/4-1);

        int key;
        printf("\n[A] Print your search key: ");
        scanf("%d",&key);
        int ans=searchTree(root,key);
        if(ans)printf("Success!\n");
        else printf("Failed!\n");
        close(pipe_a_to_b[1]); 
        close(pipe_b_to_a[0]); 
    }
    else { // 子进程
        close(pipe_a_to_b[1]); // 关闭A->B写
        close(pipe_b_to_a[0]); // 关闭B->A读
        
        
        int received_array[5];
        read(pipe_a_to_b[0], received_array, sizeof(received_array));
        printf("[B] Received array from A: ");
        for (int i = 0; i < 5; ++i) {
            printf("%d ", received_array[i]);
        }
        printf("\n");
        printf("[B] Send preorder and inorder to A!\n");
        TreeNode *root=buildBinaryTree(received_array,sizeof(received_array));
        preorder(root,pipe_b_to_a[1]);
        inorder(root,pipe_b_to_a[1]);
        

        close(pipe_a_to_b[0]);
        close(pipe_b_to_a[1]);
    }
    
    return 0;
}
