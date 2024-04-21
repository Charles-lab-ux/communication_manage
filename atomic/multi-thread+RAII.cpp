#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>
#define MAX_COUNT 100
#define NUM_THREADS 10

int count = 0;
std::mutex mtx;

void increment(int tid) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        //泄露锁，在生命周期结束自动释放，可以手动解锁
        if (count >= MAX_COUNT) {
            // 不需要手动释放
            break;
        }
        count++;
        std::cout << "Thread " << tid << ": Count = " << count << std::endl;
        mtx.unlock(); // 释放锁
        sleep(3);
    }
}

int main() {
    std::thread threads[NUM_THREADS];
    // 创建多个线程
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i] = std::thread(increment, i);
    }
    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }
    std::cout << "Final Count: " << count << std::endl;
    return 0;
}
