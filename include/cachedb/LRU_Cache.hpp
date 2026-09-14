// LRU - Least Recently used.
// Front of the List stores most recent
// Back of List stores least recent
#include<unordered_map>
#include<list>
#include<iostream>
#include<optional>
#include<mutex>
#include<chrono>
#include<thread>
#include<atomic>
#include<condition_variable>
using namespace std;

template<typename Key, typename Value>
class LRUCache{
    public:
    using Clock = chrono::steady_clock;
    using TimePoint = Clock::time_point;
    struct Entry{
       Key key;
       Value value;
       TimePoint expiry;
   };
    private:
    size_t  capacity;
    list<Entry>lst;
    unordered_map<Key,  typename list<Entry>::iterator>mp;
    mutex mutex_;
    condition_variable cv;
    atomic<bool>stop{false};
    thread purgeThread;
    void prugeLoop(int purgeIntervalMs){
        unique_lock<mutex>lock(mutex_);
        while(!stop){
            cv.wait_for(lock,chrono::milliseconds(purgeIntervalMs), [this]{return stop.load();});
            if(stop)break;

            auto now = Clock::now();
            auto it = lst.begin();
            while(it != lst.end()){
                if(now >= it->expiry){
                    mp.erase(it->key);
                    it = lst.erase(it);
                }else{
                    it++;
                }
            }

        }
    }
    public:
   
   
    explicit LRUCache(size_t capacity, int purgeIntervalms = 1000):capacity(capacity){
        purgeThread = thread(&LRUCache::prugeLoop, this, purgeIntervalms);
    };
    optional<Value> get(const Key& key){
        lock_guard<mutex>lock(mutex_);
        auto it = mp.find(key);
        if(it == mp.end()){
            return nullopt;
        }
        if(Clock::now() >= it->second->expiry){
            lst.erase(it->second);
            mp.erase(it);
            return nullopt;
        }
        Value val = it->second->value;
        TimePoint expiry = it->second->expiry;
        lst.erase(it->second);
        lst.push_front({key, val,expiry});
        mp[key] = lst.begin();
        return val;
    }
    void put(const Key& key, const Value& val, int ttl_seconds = 0){
        lock_guard<mutex>lock(mutex_);
        TimePoint expiry;
        if(ttl_seconds > 0)expiry = Clock::now() + chrono::seconds(ttl_seconds);
        else expiry = TimePoint::max();

        auto it = mp.find(key);
        if(it != mp.end()){
            lst.erase(it->second);
            lst.push_front(Entry{key, val, expiry});
            mp[key] = lst.begin();
            return ;
        }
        lst.push_front(Entry{key, val, expiry});
        mp[key] = lst.begin();

        if(mp.size() > capacity){
            auto& node = lst.back();
            mp.erase(node.key);
            lst.pop_back();
        }
    }
    size_t size(){
        lock_guard<mutex>lock(mutex_);
        return mp.size();
    }
    void debugPrint(){
        cout<<"Printing LRU from Most Recent to Least Recent."<<endl;
        for(const auto& p: lst){
            cout<<"Key: "<<p.key<<" Value: "<<p.value<<endl;
        }
        cout<<endl;
    }
};

