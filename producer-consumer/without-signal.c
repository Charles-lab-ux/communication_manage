#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unistd.h>
#include <ctime>

#define MAX_SIZE 10

using namespace std;
vector<vector<int>> taskQueue(MAX_SIZE);
int front=0,rear=0;
mutex mtx;
void producer(){
    while(true){
        vector<int> arr;
        for(int j=0;j<10;j++)arr.push_back(rand()%100);
        mtx.lock();
        if((rear+1)%MAX_SIZE==front){
            //队列已满 等待
            mtx.unlock();
        }else{
            taskQueue[rear]=arr;
            rear=(rear+1)%MAX_SIZE;
            mtx.unlock();
            cout<<"Producer produced array: ";
            for(int num:arr)cout<<num<<" ";
            cout<<endl;
        }
        sleep(3);
    }
}
void consumer(int id){
    while(true){
        mtx.lock();
        if(front==rear){
            //队列为空 等待
            mtx.unlock();
        }else{
            vector<int> arr=taskQueue[front];
            front=(front+1)%MAX_SIZE;
            mtx.unlock();
            sort(arr.begin(),arr.end());
            // 输出到文件
            ofstream outFile("result1.txt", ios::app);
            outFile << "Thread " << id << " sorted array: ";
            for (int num : arr) {
                outFile << num << " ";
            }
            outFile << endl;

            cout << "Thread " << id << " sorted array: ";
            for (int num : arr)cout << num << " ";
            cout << endl;
            sleep(3);
        }
    }
}
int main(){
    srand(time(nullptr));
    thread producerThread(producer);
    vector<thread> consumerThreads;
    for (int i = 0; i < 5; ++i)
        consumerThreads.emplace_back(consumer, i);
    producerThread.join();
    for (auto& t : consumerThreads) {
        t.join();
    }
    return 0;
}