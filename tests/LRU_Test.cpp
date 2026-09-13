#include "../include/cachedb/LRU_Cache.hpp"
#include<cassert>
#include<iostream>
#include<string>
using namespace std;
int main(){

    LRUCache<int, std::string> cache(3);
 
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    cache.debugPrint();

    auto val = cache.get(1);
    assert(val.has_value() && val.value() == "one");
    cache.debugPrint();
 
    cache.put(4, "four");
    cache.debugPrint(); 
 

    auto missing = cache.get(2);
    assert(!missing.has_value());
    std::cout << "key 2 correctly evicted (cache miss)\n";
 
  
    auto val3 = cache.get(3);
    assert(val3.has_value() && val3.value() == "three");
    std::cout << "key 3 still present: " << val3.value() << "\n";

    cache.put(1, "ONE-UPDATED");
    auto updated = cache.get(1);
    assert(updated.value() == "ONE-UPDATED");
    std::cout << "key 1 updated correctly: " << updated.value() << "\n";
 
    std::cout << "\nAll LRU Cache all tests passed.\n";
    return 0;
}