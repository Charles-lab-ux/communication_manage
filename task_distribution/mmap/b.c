#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct TreeNode {
    int data;
    int left;
    int right;
} TreeNode;

#define SHM_KEY 1234
#define SHM_SIZE 1024
void insert(TreeNode *tree,int index,int value,int last){
    tree[index].data=value;
    if(2*index+1<=last)tree[index].left=2*index+1;
    else tree[index].left=-1;
    if(2*index+2<=last)tree[index].right=2*index+2;
    else tree[index].right=-1;
    printf("%d %d %d\n",tree[index].data,tree[index].left,tree[index].right);
}
void inorder(TreeNode *tree, int index) {
    if (index != -1) {
        inorder(tree, tree[index].left);
        printf("%d\n", tree[index].data);
        inorder(tree, tree[index].right);
    }
}
int main() {
    int shm_id;
    TreeNode *shared_tree;
    void *shared_memory;
    int *array;
    int array_size=5;
    int tree_size=5;

    FILE* f_lock = NULL;
    while (!f_lock) {
        f_lock = fopen("atob.ready", "r");
    }
    fclose(f_lock);

    // 创建共享内存
    shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // 将共享内存连接到进程的地址空间
    shared_memory = shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)(-1)) {
        perror("shmat");
        exit(1);
    }

    array=(int *)shared_memory;
    shared_tree=(TreeNode *)(array+array_size);
    // 将二叉树写入共享内存
    for(int i=0;i<array_size;i++)
        insert(shared_tree,i,array[i],array_size-1);
    inorder(shared_tree,0);

    // 断开共享内存连接
    shmdt(shared_tree);
    shmdt(array);

    printf("Binary tree has been written to shared memory.\n");
    FILE* f_sorted = fopen("btoa.ready", "w");
    fclose(f_sorted);
    return 0;
}
