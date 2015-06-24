/**         MULTI THREADED FILE INDEXING SCHEME
 *	Developed by Dr. Prasanta Pal, Umass School of Medicine && Yale School of Engineering
 **/
#ifndef INDEX_CPP_H
#define INDEX_CPP_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <boost/lexical_cast.hpp>
#include <boost/signals2.hpp>

class Index {

    public:
        void IndexHelper();//Helper function to ctors
        Index();
    
        void setSourceFileName(const std::string);
        std::string getSourceFileName() const;
        bool getIsSourceFileNameSet() const;
        void resetIsSourceFileNameSet();
    
        void setTargetFileName(const std::string);
        std::string getTargetFileName() const;
        std::string getTargetFileName(const std::string) const;
        bool getIsTargetFileNameSet() const;
        void resetIsTargetFileNameSet();
    
        static unsigned int getNumThreadsOnDevice();
	void setNumThreads(const int n);
        int getNumThreads() const;
        bool getIsNumThreadsSet() const;
        void resetIsNumThreadsSet();
    
        void setReadBufferSize(const int n);
        int getReadBufferSize() const;
        bool isReadBufferSet() const;//The flag can be set only through setReadBuffer
        void resetIsReadBufferSet();//change the state to false
    
        void setIsReadyToRun();
        void resetIsReadyToRun();
    
        bool isReadyToRun() const;
        void IndexHelp();//help file
        void IndexError(const std::string);//error function
        static bool isNumber(const std::string);
	
	static void setIsVerbose(const bool is_verbose) { is_verbose_ = is_verbose; }
	static bool getIsVerbose()  { return is_verbose_; }
private:
	static bool is_verbose_;    
        std::string source_file_name_;
        bool is_source_file_name_set_;
        std::ifstream source_file_;
        std::string target_file_name_;
        bool is_target_file_name_set_;
        std::ofstream target_file_;//target file is the index file
        int num_threads_;
        bool is_num_threads_set_;
        int read_buffer_size_;
        bool is_read_buffer_set_;
        bool is_ready_to_run_;//The indexing function would run based on the true value
        std::map < std::string,bool >  run_flag_list_;
        std::vector<std::string> explicit_list_of_check_flags_;
        boost::signals2::signal<void ()> state_signal_;

};

#endif

