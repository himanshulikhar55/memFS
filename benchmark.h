#include "headers.h"
#include <fstream>
#define NUM_OF_WORDS 10000

std::vector<std::string> data;

std::string generate_str(){
    std::string str = "";
    int len = (rand()%200)+1;
    for(int i=0;i<len;i++)
        str += data[rand()%NUM_OF_WORDS] + " ";
    return str;
}

void create_data(){
    int num_data_pts[3] = {1000, 10000, 100000};
    std::string word;
    int op, num;
    srand(time(0));
    std::ifstream fin = std::ifstream("corpus.txt");
    while(fin >> word){
        data.push_back(word);
    }
    for(int i=0;i<3;i++){
        int count[4] = {0,0,0,0};
        int tot = 0;
        std::ofstream fout = std::ofstream("data_"+std::to_string(num_data_pts[i])+".txt");
        while(tot < 4*num_data_pts[i]){
            op = rand()%4;
            num = rand()%num_data_pts[i];
            
            /* num_data_pts[i] creates */
            if(op == 0){
                fout << "create file" << num << ".txt\n";
                count[0]++;
                tot++;
            }
            /* num_data_pts[i] writes */
            if(op == 1){
                std::string str = generate_str();
                fout << "write file" << num << ".txt#" << str << "\n";
                count[1]++;
                tot++;
            }
            /* num_data_pts[i] reads */
            if(op == 2){
                fout << "read file" << num << ".txt\n";
                count[2]++;
                tot++;
            }
            /* num_data_pts[i] deletes */
            if(op == 3){
                fout << "delete file" << num << ".txt\n";
                count[3]++;
                tot++;
            }
        }
        fout.close();
        // std::cout << "DONE\n";
    }
    
    data.clear();
}

// Get the total CPU time from /proc/stat
unsigned long long get_total_cpu_time() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);

    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    std::istringstream iss(line);
    std::string cpu;
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    return user + nice + system + idle + iowait + irq + softirq + steal;
}

unsigned long long get_process_cpu_time() {
    std::ifstream file("/proc/self/stat");
    std::string ignore;
    unsigned long long utime, stime;

    for (int i = 0; i < 13; ++i)
        file >> ignore;
    
    file >> utime >> stime;
    return utime + stime;
}

unsigned long long get_memory_usage() {
    std::ifstream file("/proc/self/status");
    std::string line;

    while (std::getline(file, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(line);
            std::string label;
            unsigned long long memory_kb;
            iss >> label >> memory_kb;
            return memory_kb;
        }
    }
    return 0;
}