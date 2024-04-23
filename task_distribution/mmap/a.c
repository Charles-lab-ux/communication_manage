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

void inorder(TreeNode *tree, int index) {
    if (index != -1) {
        inorder(tree, tree[index].left);
        printf("%d\n", tree[index].data);
        inorder(tree, tree[index].right);
    }
}
int main() {
    int shm_id;
    int *array;
    TreeNode *shared_tree;
    void *shared_memory;
    int array_size=5;
    int tree_size=5;

    // 获取共享内存的标识符
    shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // 将共享内存连接到进程的地址空间
    shared_memory=shmat(shm_id,NULL,0);
    if (shared_memory == (void *)(-1)) {
        perror("shmat");
        exit(1);
    }
    //将数组写入
    array=(int *)shared_memory;
    int nums[]={1,2,3,4,5};
    for(int i=0;i<array_size;i++)
        array[i]=nums[i];
    
    FILE* atob = fopen("atob.ready", "w");
    fclose(atob);
    FILE* f_lock = NULL;
    while (!f_lock) {
        f_lock = fopen("btoa.ready", "r");
    }
    fclose(f_lock);

    // 从共享内存中读取二叉树
    shared_tree=(TreeNode *)(array+array_size);
    inorder(shared_tree,0);
    shmdt(shared_tree);
    shmdt(array);

    return 0;
}
// ipcs -m可以查看共享内存的大小
