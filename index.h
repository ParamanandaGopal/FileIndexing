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
#include <cstdio>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/program_options.hpp>
class Index {
	public:
		void IndexHelper();//Helper function to ctors
		Index();
		char getMatchChar();
		void setSourceFileName(const std::string);
		std::string getSourceFileName() const;
		bool getIsSourceFileNameSet() const;
		void resetIsSourceFileNameSet();
		std::string createTargetFileName(const std::string);
		std::string getTargetFileName() const;
		std::string getTargetFileName(const std::string);
		void setTargetFileName(const std::string);
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
		void WARNING(const std::string warning_message);
		void add_thread(boost::shared_ptr<boost::thread>& temp_ptr){
			thread_vector_.push_back(temp_ptr);
		}
		void thread_join() {
			for(auto it:thread_vector_){
				if(it->joinable())
					it->join();
			}
		}
		void set_map(long key,long value) {
			master_index_[key]=value;
		}
	private:
		static bool is_verbose_;
		char match_char_;
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
		std::string program_path_;//path where the executable is located
		std::string home_path_;//path where the executable is located
		std::string resource_path_;//path where the executable is located
		std::vector<boost::shared_ptr<boost::thread>> thread_vector_;
		std::map<long,long> master_index_;
};
#endif
