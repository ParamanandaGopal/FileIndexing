#include "index.h"
bool Index::is_verbose_ = false;
boost::mutex Index::lock;
std::string Index::index_ext_=".idx";
unsigned Index::index_offset_=1;
unsigned Index::minimal_multithreaded_byte_limit_=1000;//who cares about multi threading for small file
std::vector<std::string> tokenize(const std::string& s, char c) {//taken from http://stackoverflow.com/questions/9435385/split-a-string-using-c11
	auto end = s.cend();
	auto start = end;

	std::vector<std::string> v;
	for( auto it = s.cbegin(); it != end; ++it ) {
		if( *it != c ) {
			if( start == end )
				start = it;
			continue;
		}
		if( start != end ) {
			v.emplace_back(start, it);
			start = end;
		}
	}
	if( start != end )
		v.emplace_back(start, end);
	return v;
}

void Index::createIndex(){
	unsigned num_threads_to_run=getNumThreadsOnDevice();
	msg("Number of available threads " + boost::lexical_cast<std::string>(num_threads_to_run));
	std::ifstream is;
	std::string file_name= getSourceFileName();
	is.open(file_name.c_str(),std::ifstream::binary);
	if(is.good()){
		is.seekg(0,is.end);
		unsigned length = is.tellg();
		std::cout << "file length is " << length << " bytes" << std::endl;
		is.seekg(0,is.beg);
		is.close();
		std::vector<std::pair<unsigned,unsigned>> thread_load_limits;
		unsigned width=length/num_threads_to_run;
		if(width <= Index::minimal_multithreaded_byte_limit_){
			num_threads_to_run=1;
			width=length;
		}

		msg("width " + boost::lexical_cast<std::string>(width));
		std::pair<unsigned,unsigned> temp_pair;
		for(unsigned i=0; i< num_threads_to_run; i++){
			temp_pair.first=i*width + 1;
			if(i==(num_threads_to_run-1))
				temp_pair.second=length;
			else
				temp_pair.second=(i+1)*width;
			thread_load_limits.push_back(temp_pair);
		}
		msg("printing limits:\n");
		for(auto it:thread_load_limits){
			msg(boost::lexical_cast<std::string>(it.first) + " " + boost::lexical_cast<std::string>(it.second));
		}
		std::map<unsigned,std::vector<unsigned>> byte_location_master;
		int count=0;
		for(auto it:thread_load_limits){
			count++;
			msg("count:" + boost::lexical_cast<std::string>(count));
			boost::shared_ptr<boost::thread> temp_ptr(
					new boost::thread([this,&byte_location_master,it,&length,count](){
						//	std::cout << "job no:" << count << std::endl;
						unsigned local_count=count;
						std::ifstream is;
						std::string file_name= getSourceFileName();
						is.open(file_name.c_str(),std::ifstream::binary);
						std::vector<unsigned> byte_location;
						if(is.good()) {
						char c[2];
						char regex='\n';
						unsigned begin=it.first;
						unsigned end=it.second;
						is.seekg(begin);
						for(unsigned i=begin;i<=end;i++){
						is.read(c,1);
						if(!is.eof()){
						if(c[0] == regex)
						{
						byte_location.push_back(is.tellg());
						//lock.lock();
						//std::cout << "job:" << local_count << ":";
						//std::copy(byte_location.begin(),byte_location.end(),std::ostream_iterator<int>(std::cout," "));

						//std::cout << std::endl;

						//lock.unlock();

						}} else {
							is.close();
						}
						}
						msg("resource lock " + boost::lexical_cast<std::string>(local_count));
						lock.lock();
						byte_location_master[local_count]=byte_location;
						//std::copy(byte_location.begin(),byte_location.end(),std::ostream_iterator<int>(std::cout," "));
						//std::cout << std::endl;
						lock.unlock();
						msg("resource unlock " + boost::lexical_cast<std::string>(local_count));
						}else {
							msg("file is not good");
							default_throw_function("error opening file " + file_name);
						}
					}));
			add_thread(temp_ptr);
		}
		msg("joining threads");
		thread_join();
		msg("thread join complete");
		unsigned long k=0;
		msg("printing byte locations\n");
		std::cout << byte_location_master.size() << std::endl;
		std::map<long,long> master_index_;
		for(auto it:byte_location_master){
			for(auto ptr:it.second){
				master_index_[k++]=ptr;
			}
		}
		std::cout << "saving results to index file:" << getTargetFileName() << std::endl;
		std::string header=file_name  + " " + boost::lexical_cast<std::string>(length) + " " + boost::lexical_cast<std::string>(master_index_.size()) +  " " + "\\n";
		print_map(header,master_index_,getTargetFileName());
	}else {
		default_throw_function("error opening file " + file_name);
	}

}

void Index::to_cout(const std::vector<std::string> &v)
{
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>{
			std::cout, "\n"});
}
void Index::msg(std::string str) { std::cout << str << std::endl;}
void Index::default_throw_function(std::string str){
	throw str;
}
void Index::print_map(std::string header, std::map<long,long> map,std::string filename){
	std::ofstream file(filename.c_str());
	if(file.good()){
		file << header << std::endl;
		for(auto it:map){
			file << (it.first + index_offset_) << " " << it.second << std::endl;
		}
	} else {
		default_throw_function("can not open file " + filename + " for writting");
	}
}

void Index::WARNING(const std::string warning_message) {
	std::cout << "WARNING:" << warning_message << std::endl;
}
char Index::getMatchChar() {
	return match_char_;
}
void Index::IndexHelper(){//Helper functions for ctors
	match_char_='\n';
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
	std::string program_path_temp(getenv("PWD"));
	program_path_=program_path_temp;
	std::string home_path_temp(getenv("HOME"));
	home_path_=home_path_temp;
	resource_path_=program_path_;
	std::cout << "program_path_:" << program_path_ << std::endl;
	std::cout << "home_path_:" << home_path_ << std::endl;
	std::cout << "resource_path_:" << resource_path_ << std::endl;
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
	run_flag_list_[current_flag] = is_target_file_name_set_;
	state_signal_();
}
std::string Index::getTargetFileName() const {
	if(is_target_file_name_set_)
		return target_file_name_;
	else throw "Exception at getTargetFileName... variable target_file_name_ not set";
}
std::string Index::createTargetFileName(const std::string source_file_name) {
	setSourceFileName(source_file_name);
	std::string target_file_name_temp=source_file_name;
	auto target_filename_vector=tokenize(target_file_name_temp,'.');
	if(target_filename_vector.size() > 0){
		target_file_name_temp=target_filename_vector[0] + index_ext_;
	}
	std::cout << "target_file_name_temp " << target_file_name_temp << std::endl;

	setTargetFileName(target_file_name_temp);

	return target_file_name_;
}
std::string Index::createTargetFileName() {
	createTargetFileName(source_file_name_);
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
	if(is_verbose_)
		std::cout << "calling setIsReadyToRun" << std::endl;
	for(auto i : explicit_list_of_check_flags_ ){
		if( run_flag_list_[i] != true){
			is_ready_to_run_ = false;
			std::cout << "flag " << i << " is not set" << std::endl;
			return;
		}
	}
	is_ready_to_run_ = true;
}
bool Index::isReadyToRun() const {
	return is_ready_to_run_;
}
void Index::IndexHelp(){
	std::cout << "Welcome to file indexing help" << std::endl;
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
