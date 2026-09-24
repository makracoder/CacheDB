#include "../include/cachedb/ThreadPool.hpp"
#include <iostream>
#include <atomic>
#include <chrono>
#include <vector>
using namespace std;
int main() {

    atomic<int> counter{0};
    const int NUM_TASKS = 100000;
    {
        ThreadPool pool(8);
        for (int i = 0; i < NUM_TASKS; ++i) {
            pool.enqueue([&counter]() {
                counter.fetch_add(1);
            });
        }

    }
    cout << "Test 1 -- Correctness:\n";
    cout << "  Submitted " << NUM_TASKS << " tasks, counter ended at "
              << counter.load() << "\n";
    cout << "  " << (counter.load() == NUM_TASKS ? "PASS" : "FAIL") << "\n\n";
    {
        const int NUM_SLOW_TASKS = 32;
        const int TASK_DURATION_MS = 50;
        const int NUM_WORKERS = 8;

        auto start = chrono::steady_clock::now();
        {
            ThreadPool pool(NUM_WORKERS);
            for (int i = 0; i < NUM_SLOW_TASKS; ++i) {
                pool.enqueue([TASK_DURATION_MS]() {
                    this_thread::sleep_for(chrono::milliseconds(TASK_DURATION_MS));
                });
            }
        } 
        auto end = chrono::steady_clock::now();

        auto elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        long sequential_would_take = (long)NUM_SLOW_TASKS * TASK_DURATION_MS;
        long ideal_parallel_time = sequential_would_take / NUM_WORKERS;

        cout << "Test 2 -- Parallelism:\n";
        cout << "  " << NUM_SLOW_TASKS << " tasks x " << TASK_DURATION_MS << "ms each, "
                  << NUM_WORKERS << " workers\n";
        cout << "  If run one-by-one (sequential): ~" << sequential_would_take << "ms\n";
        cout << "  Ideal parallel time: ~" << ideal_parallel_time << "ms\n";
        cout << "  Actual measured time: " << elapsed_ms << "ms\n";

        if (elapsed_ms < sequential_would_take / 2) {
            cout << "  PASS: tasks clearly ran in parallel, not one-by-one.\n";
        } else {
            cout << "  FAIL: looks like tasks ran sequentially, not in parallel.\n";
        }
    }

    return 0;
}