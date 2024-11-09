#include "headers.h"
/**
 * Command types:
 * 1: Create
 * 2: Write
 * 3: Delete
 * 4: Read
 * 5: ls
 * 6: exit
 */
class command{
    private:
        int comm_type;
        int arg_count;
        /* filename */
        std::vector<std::string> args;
        /* string */
        std::vector<std::string> argv;
        bool is_invalid;
    public:
        command(int comm_type){
            this->comm_type = comm_type;
            this->arg_count = 0;
            this->is_invalid = false;
        }
        bool invalid(){
            return this->is_invalid;
        }
        int get_comm_type(){
            return this->comm_type;
        }
        std::vector<std::string>& get_args(){
            return this->args;
        }
        std::vector<std::string>& get_argv(){
            return this->argv;
        }
        int get_arg_count(){
            return this->arg_count;
        }
        void set_command(int comm_type){
            this->comm_type = comm_type;
        }
        void set_invalid(){
            this->is_invalid = true;
        }
        void set_valid(){
            this->is_invalid = false;
        }
        void set_filename_size(int size, int comm_type){
            this->args = std::vector<std::string>(size);
            if(comm_type == 2){
                this->argv = std::vector<std::string>(size);
            }
        }
        void set_arg_count(int arg_count){
            this->arg_count = arg_count;
        }
};