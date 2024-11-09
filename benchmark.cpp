#include "thread_pool.h"
#include "interpreter.h"
#include "memFS.h"
#include "benchmark.h"

int main(){
    
    auto last_total_cpu_time = get_total_cpu_time();
    auto last_process_cpu_time = get_process_cpu_time();

    std::string ip, op;
    int n = 5;
    int arr[5] = {1,2,4,8,16};
    int num_data_pts[3] = {1000, 10000, 100000};
    std::vector<double> time_taken;
    sem_init(&memFS_mutex, 0, 1);
    create_data();
    double cpu_mem_stats[5][3];
    double memory_stats[5][3];
    double latency_stats[5][3];
    double tot_lat=0, tot_num=0;
    std::unordered_map<std::string, file_info*> memFS;
    for(int i=0;i<5;i++){
        printf("---------------------------------------------------------------------------------------------\n");
        printf("Threads: %d\n", arr[i]);
        printf("---------------------------------------------------------------------------------------------\n");
        for(int j=0;j<3;j++){
            memFS = std::unordered_map<std::string, file_info*>();
            printf("Data Points: %d\n", num_data_pts[j]*4);
            thread_pool *pool = new thread_pool(arr[i], true);
            pool->start_pool();
            std::ifstream fin = std::ifstream("data_"+std::to_string(num_data_pts[j])+".txt");
            std::string line;
            std::string temp;
            auto start = std::chrono::high_resolution_clock::now();
            while(std::getline(fin, line)){
                std::stringstream ss(line);
                thread_data *data = new thread_data(&memFS);
                ss >> temp;
                ss >> data->filename;
                if(temp[0] == 'c'){
                    data->comm_type = 1;
                }
                else if(temp[0] == 'w'){
                    data->comm_type = 2;
                    data->filename = line.substr(temp.length()+1);
                    /* Remove the last space */
                    data->filename.pop_back();
                    // std::cout << data->filename << " " << std::flush;
                } else if (temp[0] == 'r'){
                    data->comm_type = 4;
                } else {
                    data->comm_type = 3;
                }
                pool->add_task(data);
            }
            /* printing files after each iteration */
            thread_data *data = new thread_data(&memFS);
            data->comm_type = 5;
            data->filename = "-l";
            pool->add_task(data);
            while(!pool->all_tasks_completed());
            // std::this_thread::sleep_for(std::chrono::seconds(5));
            auto end = std::chrono::high_resolution_clock::now();
            latency_stats[i][j] = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            auto total_cpu_time = get_total_cpu_time();
            auto process_cpu_time = get_process_cpu_time();

            double cpu_usage = 100.0 * (process_cpu_time - last_process_cpu_time) / (total_cpu_time - last_total_cpu_time);
            cpu_mem_stats[i][j] = cpu_usage;
            double memory_usage_kb = get_memory_usage();
            memory_stats[i][j] = memory_usage_kb/1024;
            // op = std::move("CPU Usage by process: " + std::to_string(cpu_usage) + "%\n");
            // printf("%s", op.c_str());
            // op = std::move("Memory Usage by process: " + std::to_string(memory_usage_kb / 1024) + " KB\n");
            // printf("%s", op.c_str());
            
            fflush(stdout);
            // Update previous values
            last_total_cpu_time = total_cpu_time;
            last_process_cpu_time = process_cpu_time;
            for (auto &pair : memFS) {
                delete pair.second;
            }
            memFS.clear();
            /* destructor has stop pool called in it. It will take care of it all */
            delete pool;
        }
    }

    printf("---------------------------------------------------------------------------------------------\n");
    printf("---------------------------------------------------------------------------------------------\n");
    for(int i=0;i<5;i++){
        printf("|                                        Threads: %d                                         |\n", arr[i]);
        printf("---------------------------------------------------------------------------------------------\n");
        printf("---------------------------------------------------------------------------------------------\n");
        for(int j=0;j<3;j++){
            tot_lat += latency_stats[i][j];
            tot_num += num_data_pts[j]*4;
            printf("Data Points: %d\n", num_data_pts[j]*4);
            printf("Latency: %lf ms, Memory Used: %lf MB, CPU Usage: %lf%%\n", latency_stats[i][j], memory_stats[i][j], cpu_mem_stats[i][j]);
            printf("Total time for %d operations is: %lfms\n", num_data_pts[j], latency_stats[i][j]);
            printf("---------------------------------------------------------------------------------------------\n");
        }
        printf("---------------------------------------------------------------------------------------------\n");
    }

    printf("Average time taken for an operation is: %lfms\n", tot_lat/tot_num);

    

    return 0;
}
