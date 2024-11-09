#pragma once
#include <vector>
#include <string>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <semaphore.h>
#include <atomic>
#include <thread>
#include <condition_variable>
#include "command.h"
#include "file_info.h"

struct thread_data {
    int comm_type;
    std::string filename;
    std::unordered_map<std::string, file_info*> *memFS;
    int ret_val;
    thread_data(){
        comm_type = -1;
        filename = "";
        memFS = NULL;
        ret_val = -1;
    }
    thread_data(std::unordered_map<std::string, file_info*> *m){
        comm_type = -1;
        filename = "";
        memFS = m;
        ret_val = -1;
    }
};

void create_file(thread_data *data);
void write_to_file(thread_data *data);
void delete_file(thread_data *data);
void read_file(thread_data *data);
void list_files(thread_data *data);

void (*memFS_func[])(thread_data*) = {create_file, write_to_file, delete_file, read_file, list_files};

sem_t memFS_mutex;