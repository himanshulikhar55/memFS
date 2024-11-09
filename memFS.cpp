#include "interpreter.h"
#include "memFS.h"
#include "thread_pool.h"

int main(){
    std::string ip;
    std::unordered_map<std::string, file_info*> memFS;
    thread_pool pool(8);
    pool.start_pool();
    // initialize memFS_mutex
    sem_init(&memFS_mutex, 0, 1);
    while(1){
        std::cout << "memFS> ";
        getline(std::cin, ip);
        command *c = new command(-1);
        if(ip.compare("exit") == 0){
            std::cout << "exiting memFS\n";
            delete c;
            break;
        }
        split_command(ip, *c);
        if(c->invalid()){
            std::cout << "Invalid command\n";
            delete c;
            continue;
        }
        int n = c->get_args().size();
        int cnt = 0, cmd = c->get_comm_type();
        std::vector<thread_data> data(n);
        std::vector<std::string> args = std::move(c->get_args());
        std::vector<std::string> argv = std::move(c->get_argv());
        std::string err_fnames = "";
        for(int i = 0; i < n; i++){
            data[i].memFS = &memFS;
            data[i].comm_type = cmd;
            if(cmd == 2){
                data[i].filename = args[i] + "#" + std::move(argv[i]);
            } else{
                data[i].filename = std::move(args[i]);
            }
            // std::cout << data[i].filename << " " << "cmd: " << cmd << '\n';
            pool.add_task(&data[i]);
        }
        /* Wait until all tasks complete */
        while(!pool.all_tasks_completed());
        for(int i=0;i<n;i++){
            if(data[i].ret_val == 0 && cmd != 4){
                err_fnames += std::move(data[i].filename) + " ";
            }
            cnt += data[i].ret_val;
        }
        if(cmd == 1){
            if(cnt == n){
                if(n>1)
                    std::cout << "files created successfully\n";
                else
                    std::cout << "file created successfully\n";
            } else {
                if(n == 1)
                    std::cout << "error: another file with same name exists\n";
                else
                    std::cout << "error: some files already exist\n";
            }
        } else if(cmd == 2){
            if(cnt == n){
                if(n>1)
                    std::cout << "successfully written to the given files\n";
                else
                    std::cout << "successfully written to " << args[0] << "\n";
            } else {
                if(n == 1)
                    std::cout << "error: " << args[0] << " file does not exist\n";
                else
                    std::cout << "error: some files do not exist\n";
            }
        } else if(cmd == 3){
            if(cnt == n){
                if(n>1)
                    std::cout << "Files deleted successfully\n";
                else
                    std::cout << "file deleted successfully\n";
            } else {
                if(n == 1)
                    std::cout << "error: " << err_fnames << "file does not exist\n";
                else
                    std::cout << "File" << err_fnames << "don't exist. remaining files deleted successfully\n";
            }
        } else {
            std::cout << data[0].filename;
        }
        delete c;
    }

    pool.stop_pool();
    for (auto &pair : memFS) {
        delete pair.second;
    }
    memFS.clear();
    sem_destroy(&memFS_mutex);
    return 0;
}