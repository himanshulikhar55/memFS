#include "headers.h"

const std::string commands[] = {"create", "write", "delete", "read", "ls", "exit"};

bool valid_command(std::string s, command &c){
    for(int i=0;i<6;i++){
        if(s.compare(commands[i]) == 0){
            c.set_command(i+1);
            return true;
        }
    }
    return false;
}

bool is_number(std::string &s){
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

void create_delete_command(const std::string &s, command &c){
    std::string word;
    std::stringstream ss(s);
    std::vector<std::string>& args = c.get_args();
    /* since we have arrived here, we are sure that command is correct until first 3 words */
    ss >> word;
    ss >> word;
    ss >> word;

    int num = c.get_arg_count(), i=0;
    while(ss >> word && i<num){
        args[i++] = std::move(word);
    }
}

void write_command(const std::string &s, command &c){
    std::string word;
    std::stringstream ss(s);
    std::vector<std::string>& argv = c.get_argv();
    std::vector<std::string>& args = c.get_args();
    /* since we have arrived here, we are sure that command is correct until first 3 words */
    ss >> word;
    ss >> word;
    ss >> word;

    int num = c.get_arg_count(), i=0, turn = 0;
    while(ss >> word && i<num){
        if(turn){
            /* this is string */
            std::string temp = word.substr(1, word.length()-1);
            if(word[word.length()-1] == '\"'){
                argv[i++] = temp.substr(0, temp.length()-1);
                turn = 0;
                continue;
            }
            while(ss >> word){
                temp += " " + word;
                if(word[word.length()-1] == '\"'){
                    break;
                }
            }
            std::string sub_str = temp.substr(0, temp.length() - 1);
            argv[i++] = std::move(sub_str);
            turn = 0;
        } else {
            /* this is filename */
            args[i] = std::move(word);
            turn = 1;
        }
    }
}

void split_command(std::string s, command &c){
    std::string word, comm;
    std::stringstream ss(s);
    int count = 0, num = 0;
    
    /* Extract words from the sentence */
    ss >> word;
    /* First check if the command name is valid */
    if(!valid_command(word, c)){
        // std::cout << "HERE\n";
        c.set_invalid();
        return;
    }
    
    /* now check if there is option present or not */
    ss >> word;
    if(!ss){
        if(c.get_comm_type() != 5){
            c.set_invalid();
        } else {
            c.set_filename_size(1, c.get_comm_type());
            c.get_args()[0] = std::move(word);
        }
        return;
    }
    if(word.compare("-n") == 0){
        if(c.get_comm_type() > 3){
            // std::cout << "HERE1\n";
            c.set_invalid();
            return;
        }
        
        /* read the number */
        ss >> word;
        
        /* Check if the number is valid. -ve numbers are also invalid */
        if(!is_number(word) || std::stoi(word) == 0){
            // std::cout << "HERE2\n";
            c.set_invalid();
            return;
        }

        /* Set the arg_count and args */
        num = std::stoi(word);
        c.set_filename_size(num, c.get_comm_type());
        c.set_arg_count(num);
    } else if(word.compare("-l") == 0){
        /* -l is onlt supported for ls command */
        if(c.get_comm_type() != 5){
            // std::cout << "HERE3\n";
            c.set_invalid();
            return;
        }
        c.get_args().push_back(std::move(word));
        c.set_arg_count(1);
        return;
    } else if(word.length()>0 && word[0] == '-'){
        c.set_invalid();
        return;
    }
    
    /**
     * If arg_count is zero, we are sure to only have only one file. We will only process only
     * one file even if the user enters more.
     */

    if(c.get_arg_count() == 0){
        c.set_filename_size(1, c.get_comm_type());
        c.get_args()[0] = std::move(word);
        if(c.get_comm_type() == 2){
            if(ss >> word){
                std::vector<std::string>& argv = c.get_argv();
                std::string temp = word;
                if(word[word.length()-1] == '\"'){
                    argv[0] = std::move(temp.substr(1, temp.length()-2));
                    return;
                }
                while(ss >> word){
                    temp += " " + word;
                    if(word[word.length()-1] == '\"'){
                        break;
                    }
                }
                std::string sub_str = temp.substr(1, temp.length()-2);
                argv[0] = std::move(sub_str);
            }
            else
                c.set_invalid();
        }
        return;
    }

    /**
     * Instead of keeping many if conditions or switch case statements, better to
     * call separate functions for each command
     */
    if(c.get_comm_type() == 1 || c.get_comm_type() == 3){
        create_delete_command(s, c);
    } else if(c.get_comm_type() == 2){
        write_command(s, c);
    }

    /* Now we will input till we get arg_count filenames and arg_count strings (if needed) */
}