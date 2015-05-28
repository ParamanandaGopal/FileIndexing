#include "index.h"


void Index::IndexHelper(){//Helper functions for ctors
	state_signal_.connect(boost::bind(&Index::setIsReadyToRun,this));

	std::string current_flag = "";

	is_source_file_name_set_ = false;
	current_flag = "is_source_file_name_set_";
	run_flag_list_[current_flag] = is_source_file_name_set_;
	explicit_list_of_check_flags_.push_back(current_flag);

	is_target_file_name_set_ = false;
	current_flag = "is_target_file_name_set_";
	run_flag_list_[current_flag] = is_target_file_name_set_;
	explicit_list_of_check_flags_.push_back(current_flag);

	num_threads_ = 1;//Default config
	is_num_threads_set_ = true;
	current_flag = "is_num_threads_set_";
	run_flag_list_[current_flag] = is_num_threads_set_;
	explicit_list_of_check_flags_.push_back(current_flag);
	is_ready_to_run_ = false;

}

Index::Index(){//ctor
	IndexHelper();
}

void Index::setSourceFileName(const std::string source_file_name){
	std::string current_flag = "";
	source_file_name_ = source_file_name;
	is_source_file_name_set_ = true;

	current_flag = "is_source_file_name_set_";
	run_flag_list_[current_flag] = is_source_file_name_set_;

	state_signal_();
}

std::string Index::getSourceFileName() const {
	return source_file_name_;
}

bool Index::getIsSourceFileNameSet() const {

	return is_source_file_name_set_;
}

void Index::resetIsSourceFileNameSet(){
	is_source_file_name_set_ = false;
}


void Index::setTargetFileName(const std::string target_file_name){
	std::string current_flag = "";
	target_file_name_ = target_file_name;
	is_target_file_name_set_ = true;

	current_flag = "is_target_file_name_set_";
	run_flag_list_[current_flag] = is_source_file_name_set_;

	state_signal_();
}

std::string Index::getTargetFileName() const {
	return target_file_name_;
}

bool Index::getIsTargetFileNameSet() const {
	return is_target_file_name_set_;
}

void Index::resetIsTargetFileNameSet(){
	is_target_file_name_set_ = false;

}

unsigned Index::getNumThreadsOnDevice() {//calculate and return the number of threads available on the hardware
	 return std::thread::hardware_concurrency();
}
void Index::setNumThreads(const int n){
	std::string current_flag = "";
	num_threads_ = n;
	if(n > 0 ){
		is_num_threads_set_ = true;
		current_flag = "is_num_threads_set_";
		run_flag_list_[current_flag] = is_source_file_name_set_;
	}else {
		is_num_threads_set_ = false;
	}

	state_signal_();

}

int Index::getNumThreads() const {
	if(is_num_threads_set_)
		return num_threads_;
	else 
		throw "num_threads_ variable is not set";
}

bool Index::getIsNumThreadsSet() const {
	return is_num_threads_set_;
}

bool Index::isReadBufferSet() const {
	return is_read_buffer_set_;
}

void Index::resetIsReadBufferSet(){
	is_read_buffer_set_ = false;
}

void Index::setIsReadyToRun(){//Check for all conditionals

	for(auto i : explicit_list_of_check_flags_ ){ 

		if( run_flag_list_[i] != true){

			is_ready_to_run_ = false;
			return;
		}

	}

	is_ready_to_run_ = true;
}

bool Index::isReadyToRun() const {
	return is_ready_to_run_;
}

void Index::IndexHelp(){

	std::cout << "Welcome to file indexing" << std::endl;

	std::cout << "ctors Index()" << std::endl;

	if(getIsSourceFileNameSet())
		std::cout << "source file-->" << getSourceFileName() << std::endl;

	if(getIsTargetFileNameSet())
		std::cout << "source file-->" << getTargetFileName() << std::endl;

	if(getIsNumThreadsSet())
		std::cout << "number of threads-->" << getNumThreads() << std::endl;




}

void Index::IndexError(const std::string s){
	std::cout << s << std::endl;
}
bool Index::isNumber(const std::string s){
	return !s.empty() && std::find_if(s.begin(), 
			s.end(), [](char c) { return !std::isdigit(c); }) == s.end();

}
