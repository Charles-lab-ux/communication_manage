#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <chrono>
#define MAX_COUNT 100000000
#define NUM_THREADS 10

int count = 0;
std::mutex mtx;

void increment(int tid) {
    while (true) {
        mtx.lock(); // 锁定互斥锁
        if (count >= MAX_COUNT) {
            mtx.unlock(); // 在退出线程之前释放锁
            break;
        }
        count++;
        std::cout << "Thread " << tid << ": Count = " << count << std::endl;
        mtx.unlock(); // 释放锁
    }
}

int main() {
    auto start = std::chrono::steady_clock::now(); // 记录开始时间点

    std::thread threads[NUM_THREADS];
    // 创建多个线程
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i] = std::thread(increment, i);
    }
    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }
    auto end = std::chrono::steady_clock::now(); // 记录结束时间点
    std::chrono::duration<double> elapsed_seconds = end - start; // 计算时间间隔

    std::cout << "Final Count: " << count << std::endl;
    std::cout << "Total time: " << elapsed_seconds.count() << " seconds" << std::endl; // 输出总运行时间

    return 0;
}
