// Thread Pool for Handling Requests

#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<atomic>
#include<future>
using namespace std;

class ThreadPool{
    private:
    vector<thread>workers;
    queue<function<void()>>taskQueue;
    mutex queue_mutex;
    condition_variable cv;
    atomic<bool> stop{false};
    void workerLoop(){
        while(true){
            function<void()>task;
            {
                unique_lock<mutex>lock(queue_mutex);
                cv.wait(lock,[this]{
                    return stop || !taskQueue.empty();
                }
                );
                if(stop && taskQueue.empty())return ;
                task = move(taskQueue.front());
                taskQueue.pop();
            }
            task();
        }
    }
    public:
    explicit ThreadPool(size_t threadCnt){
      for(size_t i= 0; i<threadCnt; i++){
         workers.emplace_back(&ThreadPool::workerLoop, this);
      }
    }
    void enqueue(function<void()>task){
        {
        lock_guard<mutex>lock(queue_mutex);
        taskQueue.push(move(task));
        }
        cv.notify_one();
    }
    ~ThreadPool(){
        {
            lock_guard<mutex>lock(queue_mutex);
            stop = true;
        }
        cv.notify_all();
        for(thread& worker: workers){
            if(worker.joinable()){
                worker.join();
            }
        }
    }
    size_t pendingTasks(){
        lock_guard<mutex>lock(queue_mutex);
        return taskQueue.size();
    }
};