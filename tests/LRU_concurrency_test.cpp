#include "../include/cachedb/LRU_Cache.hpp"
#include<cassert>
#include<iostream>
#include<string>
#include<atomic>
#include<thread>
#include<vector>
using namespace std;
int main(){

     LRUCache<int, int> cache(100);
 
    const int NUM_THREADS = 16;
    const int OPS_PER_THREAD = 20000;
 
    std::atomic<long> total_ops{0};
 
    auto worker = [&](int thread_id) {
        for (int i = 0; i < OPS_PER_THREAD; ++i) {
            int key = (thread_id * 1000 + i) % 200;
 
            if (i % 3 == 0) {
                cache.put(key, key * 10);
            } else {
                cache.get(key);
            }
 
            total_ops.fetch_add(1);
        }
    };
 
    std::vector<std::thread> threads;
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back(worker, t);
    }
 
    for (auto& th : threads) {
        th.join(); 
    }
 
    std::cout << "All " << NUM_THREADS << " threads completed.\n";
    std::cout << "Total operations performed: " << total_ops.load() << "\n";
    std::cout << "Final cache size: " << cache.size() << " (should be <= 100)\n";
 
    if (cache.size() <= 100) {
        std::cout << "PASS: cache size respects capacity, no corruption detected.\n";
    } else {
        std::cout << "FAIL: cache size exceeded capacity -- something is broken.\n";
    }
 
    return 0;
}