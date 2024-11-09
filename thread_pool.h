#include "headers.h"
#include "lock_free_queue.h"

class thread_pool {
public:
    thread_pool(int num_threads, bool is_benchmark = false)
        : num_threads(num_threads), is_benchmark(is_benchmark) {
        // if (!is_benchmark) {
        active_tasks.store(0, std::memory_order_relaxed);
        stop.store(false, std::memory_order_relaxed);
        // }
    }

    void add_task(thread_data* data) {
        tasks.enqueue(data);
        active_tasks.fetch_add(1, std::memory_order_relaxed);
        mutex_condition.notify_all();
    }

    void start_pool() {
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(&thread_pool::thread_func, this);
        }
    }

    bool all_tasks_completed() {
        // printf("active_tasks: %d\n", active_tasks.load(std::memory_order_acquire));
        // fflush(stdout);
        // if (!is_benchmark) {
        return active_tasks.load(std::memory_order_acquire) == 0;
        // }
        // return true;
    }

    void stop_pool() {
        stop.store(true);
        mutex_condition.notify_all();
        for (int i=0;i<threads.size();i++) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
        threads.clear();
    }

    ~thread_pool() {
        stop_pool();
    }

private:
    int num_threads;
    std::atomic<bool> stop;
    bool is_benchmark;
    std::vector<std::thread> threads;
    lock_free_queue tasks;
    std::condition_variable mutex_condition;
    std::mutex queue_mutex;
    std::atomic<int> active_tasks;

    void thread_func() {
        int cmd, cnt;
        std::string op;
        while (true) {
            thread_data* task = NULL;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                mutex_condition.wait(lock, [this, &task] {
                    return tasks.dequeue(task) || stop.load(std::memory_order_acquire);
                });

                if (stop.load(std::memory_order_acquire) && task == NULL) {
                    return;
                }
            }
            if (task) {
                memFS_func[task->comm_type - 1](task);
                active_tasks.fetch_sub(1, std::memory_order_relaxed);
            }
            
            /*
            cmd = task->comm_type;
            cnt = task->ret_val;
            if(cmd == 1){
                if(cnt){
                    op = std::move("file created successfully\n");
                } else {
                    op = std::move("error: another file with same name exists\n");
                }
            } else if(cmd == 2){
                if(cnt){
                    op = std::move("successfully written to " + task->filename + "\n");
                } else {
                    op = std::move("error: " + task->filename + " file does not exist\n");
                }
            } else if(cmd == 3){
                if(cnt)
                    op = std::move("file deleted successfully\n");
                else
                    op = std::move("error: " + task->filename + " file does not exist\n");
            } else{
                op = task->filename;
            }
            printf("%s", op.c_str());
            fflush(stdout);
            
            op.clear();
            */
        }
        return;
    }
};