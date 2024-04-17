#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unistd.h>
#include <ctime>
#include <condition_variable>
#define MAX_SIZE 100

using namespace std;
vector<vector<int>> taskQueue(MAX_SIZE);
int front=0,rear=0;
mutex mtx;
condition_variable cv;

void producer(){
    while(true){
        int arrayNum=rand()%10+1;
        unique_lock<mutex> lock(mtx);
        cv.wait(lock,[&arrayNum]{return (rear+arrayNum+1)%MAX_SIZE!=front;});
        while(arrayNum--){
            vector<int> arr;
            for(int j=0;j<10;j++)arr.push_back(rand()%100);
            taskQueue[rear]=arr;
            rear=(rear+1)%MAX_SIZE;
            
            cout<<"Producer produced array: ";
            for(int num:arr)cout<<num<<" ";
            cout<<endl;
        }
        lock.unlock();
        cv.notify_all();
        sleep(10);
    }
}
void consumer(int id){
    while(true){
        unique_lock<mutex> lock(mtx);
        cv.wait(lock,[]{return front!=rear;});
        vector<int> arr=taskQueue[front];
        front=(front+1)%MAX_SIZE;
        lock.unlock();
        cv.notify_all();
        sort(arr.begin(),arr.end());
        ofstream outFile("result2.txt", ios::app);
        outFile << "Thread " << id << " sorted array: ";
        for (int num : arr) outFile << num << " ";
        outFile << endl;

        cout << "Thread " << id << " sorted array: ";
        for (int num : arr)cout << num << " ";
        cout << endl;
        sleep(3);
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