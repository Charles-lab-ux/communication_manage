#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <algorithm>
#include<unistd.h>
#define MAX_SIZE 10
using namespace std;

queue<vector<int>> taskQueue;
mutex mtx;
sem_t emptySignal,fullSignal;

void producer() {
    while(true) { // 不停产生数组
        vector<int> arr;
        for (int j = 0; j < 10; ++j) {
            arr.push_back(rand() % 100); // 随机产生数组元素
        }

        sem_wait(&emptySignal);
        mtx.lock();
        
        taskQueue.push(arr);
        
        mtx.unlock();
        sem_post(&fullSignal);

        cout << "Producer produced array: ";
        for (int num : arr) {
            cout << num << " ";
        }
        cout << endl;
        sleep(3);
    }
}

void consumer(int id) {
    while (true) {
        sem_wait(&fullSignal);
        mtx.lock();

        if (taskQueue.empty()) {
            mtx.unlock();
            break; // 队列为空，退出
        }

        vector<int> arr = taskQueue.front();
        taskQueue.pop();
        mtx.unlock();
        sem_post(&emptySignal);

        // 排序数组
        sort(arr.begin(), arr.end());

        // 输出到文件
        ofstream outFile("result.txt", ios::app);
        outFile << "Thread " << id << " sorted array: ";
        for (int num : arr) {
            outFile << num << " ";
        }
        outFile << endl;

        cout << "Thread " << id << " sorted array: ";
        for (int num : arr) {
            cout << num << " ";
        }
        cout << endl;
        sleep(3);
    }
}

int main() {
    srand(time(nullptr));
    sem_init(&emptySignal,0,10);
    sem_init(&fullSignal,0,0);
    thread producerThread(producer);

    vector<thread> consumerThreads;
    for (int i = 0; i < 5; ++i) { // 创建4个消费者线程
        consumerThreads.emplace_back(consumer, i);
    }

    producerThread.join();
    for (auto& t : consumerThreads) {
        t.join();
    }

    return 0;
}
