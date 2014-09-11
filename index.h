#ifndef INDEX_CPP_H
#define INDEX_CPP_H
#include <iostream>
#include <fstream>
#include <sstream>

class Index {
        std::ifstream source_file_name_;
        bool is_source_file_name_set_;
        std::ifstream target_file_name_;
        bool is_target_file_name_set_;
};

#endif

