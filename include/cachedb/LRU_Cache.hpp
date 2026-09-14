// LRU - Least Recently used.
// Front of the List stores most recent
// Back of List stores least recent
#include<unordered_map>
#include<list>
#include<iostream>
#include<optional>
#include<mutex>
using namespace std;

template<typename Key, typename Value>
class LRUCache{
    private:
    size_t  capacity;
    list<pair<Key, Value>>lst;
    unordered_map<Key,  typename list<pair<Key, Value>>::iterator>mp;
    mutex mutex_;

    public:
    explicit LRUCache(size_t capacity):capacity(capacity){};
    optional<Value> get(const Key& key){
        lock_guard<mutex>lock(mutex_);
        auto it = mp.find(key);
        if(it == mp.end()){
            return nullopt;
        }
        Value val = it->second->second;
        lst.erase(it->second);
        lst.push_front({key, val});
        mp[key] = lst.begin();
        return val;
    }
    void put(const Key& key, const Value& val){
        lock_guard<mutex>lock(mutex_);
        auto it = mp.find(key);
        if(it != mp.end()){
            lst.erase(it->second);
            lst.push_front({key, val});
            mp[key] = lst.begin();
            return ;
        }
        lst.push_front({key, val});
        mp[key] = lst.begin();

        if(mp.size() > capacity){
            auto& node = lst.back();
            mp.erase(node.first);
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
            cout<<"Key: "<<p.first<<" Value: "<<p.second<<endl;
        }
        cout<<endl;
    }
};

