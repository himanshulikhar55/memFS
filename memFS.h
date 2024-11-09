#include "headers.h"

bool custom_comparator(file_info* a, file_info* b){
    if(a->size == b->size){
        return a->filename < b->filename;
    }
    return a->size < b->size;
}

void create_file(thread_data *data){
    std::string filename = std::move(data->filename);
    // std::cout << filename << '\n';
    std::unordered_map<std::string, file_info*> &memFS = *data->memFS;
    sem_wait(&memFS_mutex);
    if(memFS.find(filename) != memFS.end()){
        data->ret_val = 0;
        sem_post(&memFS_mutex);
        return;
    }
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d/%m/%Y");
    std::string date = oss.str();
    
    memFS[filename] = new file_info(filename);
    memFS[filename]->created_at = date;
    memFS[filename]->last_modified = date;
    memFS[filename]->size = 0;
    data->ret_val = 1;
    sem_post(&memFS_mutex);
}

void write_to_file(thread_data *data){
    std::string fname_data = std::move(data->filename);
    // std::cout << fname_data << '\n';
    std::unordered_map<std::string, file_info*> &memFS = *data->memFS;
    int pos = fname_data.find('#');
    std::string filename = fname_data.substr(0, pos);
    std::string str = fname_data.substr(pos+1);
    // std::cout << filename << " " << str << '\n';
    sem_wait(&memFS_mutex);
    if(memFS.find(filename) == memFS.end()){
        // std::cout << "Error: " << filename << " file does not exist\n" << std::flush;
        data->ret_val = 0;
        data->filename = std::move(filename);
        sem_post(&memFS_mutex);
        return;
    }
    // sem_wait(&memFS[filename]->mutex);
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d/%m/%Y");
    std::string date = oss.str();
    memFS[filename]->size = str.length();
    memFS[filename]->file_content = std::move(str);
    memFS[filename]->last_modified = date;
    // sem_post(&memFS[filename]->mutex);
    data->filename = std::move(filename);
    data->ret_val = 1;
    sem_post(&memFS_mutex);
}

void delete_file(thread_data *data){
    std::string filename = std::move(data->filename);
    file_info *temp;
    std::unordered_map<std::string, file_info*> *memFS = data->memFS;
    int val;
    sem_wait(&memFS_mutex);
    if(memFS->find(filename) == memFS->end()){
        data->ret_val = 0;
        data->filename = std::move(filename);
        sem_post(&memFS_mutex);
        return;
    }
    
    delete (*memFS)[filename];
    if(memFS->erase(filename) < 1){
        std::cout << "Something broke while deleting\n";
        exit(-1);
    }
    data->ret_val = 1;
    sem_post(&memFS_mutex);
}

void read_file(thread_data *data){
    std::string filename = data->filename;
    std::unordered_map<std::string, file_info*> &memFS = *data->memFS;
    sem_wait(&memFS_mutex);
    if(memFS.find(filename) == memFS.end()){
        data->ret_val = 0;
        data->filename = "Error: " + filename +  " file does not exist\n";
        sem_post(&memFS_mutex);
        return;
    }
    data->filename = memFS[filename]->file_content + '\n';
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    data->ret_val = 1;
    sem_post(&memFS_mutex);
}

void list_files(thread_data *data){
    sem_wait(&memFS_mutex);
    std::string opt = std::move(data->filename);
    std::unordered_map<std::string, file_info*> *memFS = data->memFS;
    std::unordered_map<std::string, file_info*>::iterator it = (*memFS).begin();
    std::vector<file_info*> files;
    // std::cout << "\n\n\nHERE\n\n\n";
    for (;it!=(*memFS).end();it++){
        // std::cout << it->first << ' ';
        files.push_back(it->second);
    }
    // std::cout << '\n';
    std::sort(files.begin(), files.end(), custom_comparator);
    if(opt.compare("-l") != 0){
        for(const auto& it: files){
            std::cout << it->filename << '\n' << std::flush;
        }
    }
    else{
        std::cout << "size\tcreated\t\tlast_modified\tfilename\n" << std::flush;
        std::vector<file_info*>::iterator it = files.begin();
        for(;it!=files.end();it++){
            if(*it){
                // std::cout << (*it)->file_content << std::flush;
                std::cout << (*it)->size << '\t' << (*it)->created_at << '\t' << (*it)->last_modified << '\t' << (*it)->filename << '\n' << std::flush;
            }
        }
    }
    data->ret_val = 1;
    sem_post(&memFS_mutex);
}