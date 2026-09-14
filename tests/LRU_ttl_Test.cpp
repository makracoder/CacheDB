#include "../include/cachedb/LRU_Cache.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
   
    LRUCache<int, std::string> cache(10, 500);

    // Test-1: Lazy Expiry on get
    cache.put(1, "short-lived", 1); 
    cache.put(2, "long-lived", 0);  

    auto val1_immediately = cache.get(1);
    std::cout << "Immediately after put, get(1) found = "
    << val1_immediately.has_value() << " (expect 1)\n";

    std::cout << "Sleeping 1.2 seconds so key 1 expires...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));

    auto val1_after_expiry = cache.get(1);
    std::cout << "After TTL passed, get(1) found = "
              << val1_after_expiry.has_value() << " (expect 0 -- lazy expiry caught it)\n";

    auto val2_still_there = cache.get(2);
    std::cout << "get(2) [no TTL] found = "
              << val2_still_there.has_value() << " (expect 1 -- never expires)\n";

    // Test 2: background purge removes entries even without get()
    std::cout << "\n--- Testing background purge ---\n";
    cache.put(3, "will-be-purged", 1);
    std::cout << "Cache size right after put(3): " << cache.size() << " (expect 2: keys 2 and 3)\n";

    std::cout << "Sleeping 2 seconds (TTL=1s + purge_interval=0.5s, "
                 "with margin) WITHOUT calling get()...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Cache size after sleep, no get() called: " << cache.size()
              << " (expect 1: only key 2 remains, key 3 purged automatically)\n";

    if (cache.size() == 1) {
        std::cout << "\nPASS: background thread purged the expired key automatically.\n";
    } else {
        std::cout << "\nFAIL: expired key was not purged by the background thread.\n";
    }

    return 0;
}