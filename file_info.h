struct file_info{
    int size;
    std::string created_at;
    std::string last_modified;
    std::string filename;
    std::string file_content;
    file_info(std::string filename){
        this->filename = filename;
        this->size = 0;
        this->created_at = "";
        this->last_modified = "";
        this->file_content = "";
    }
};