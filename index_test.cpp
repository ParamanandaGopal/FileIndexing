#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "index.h"
#include <tclap/CmdLine.h>
boost::mutex lock;
void msg(std::string str) { std::cout << str << std::endl;}
void default_throw_function(std::string str){
	throw str;
}

int main(int argc, char** argv) {
	int help_index = 1;
	std::string source_file_input;
	std::string target_file_input;
	int num_threads = 1;
	if(argc == 1 || (argc == 2 && (strncmp(argv[help_index],"--help",6) == 0 || strncmp(argv[help_index],"-help",5) == 0 || strncmp(argv[help_index],"help",4) == 0))){
		std::cout << "./index input_file || ./index input_file output_file || || ./index input_file output_file input_file || ./index input_file output_file number_of_threads(default 1) || ./index input_file output_file number_of_threads(default 1) buffer_size(~1MB)" << std::endl;
		return 0;
	}
	Index index;
	msg("SETTING VERBOSE MODE");
	index.setIsVerbose(true);
	msg("switching to case " + boost::lexical_cast<std::string>(argc));
	switch(argc){
		case 2:
			source_file_input = argv[1];
			index.setSourceFileName(source_file_input);
			std::cout << "setting output_file to " << index.createTargetFileName(index.getSourceFileName()) << std::endl;
			index.setSourceFileName(source_file_input);
			break;
		case 3:
			source_file_input = argv[1];
			index.setSourceFileName(source_file_input);
			target_file_input = argv[2];
			index.setTargetFileName(target_file_input);
			break;
		case 4:
			source_file_input = argv[1];
			index.setSourceFileName(source_file_input);//define on change events
			target_file_input = argv[2];
			index.setTargetFileName(target_file_input);
			//check for valid positive integer
			if(Index::isNumber(argv[3])){
				num_threads = boost::lexical_cast<int>(argv[3]);
				if(num_threads > 0)
					index.setNumThreads(num_threads);
				else {
					index.IndexError("Number of threads being " + boost::lexical_cast<std::string>(num_threads) + " is not acceptable. Program quitting.");
					return -1;
				}
			}
			else {
				index.IndexError("Number of threads is not a valid number. Program quitting.");
				return -1;
			}
			break;
	}
	//index.setIsReadyToRun();
	if(index.isReadyToRun() ){
		index.IndexHelp();
		std::cout << "program is ready to run " << std::endl;
	} else {
		std::cout << "program is not ready to run " << std::endl;
		return -1;
	}
	num_threads=index.getNumThreadsOnDevice();
	msg("Number of available threads " + boost::lexical_cast<std::string>(num_threads));

	std::ifstream is;
	std::string file_name= index.getSourceFileName();
	is.open(file_name.c_str(),std::ifstream::binary);
	if(is.good()){
		is.seekg(0,is.end);
		unsigned length = is.tellg();
		std::cout << "file length is " << length << " bytes" << std::endl;
		is.seekg(0,is.beg);
		is.close();

		std::vector<std::pair<unsigned,unsigned>> thread_load_limits;
		unsigned width=length/num_threads;
		msg("width " + boost::lexical_cast<std::string>(width));
		std::pair<unsigned,unsigned> temp_pair;
		for(unsigned i=0; i< num_threads; i++){
			temp_pair.first=i*width + 1;
			temp_pair.second=(i+1)*width;
			thread_load_limits.push_back(temp_pair);
		}
		if(temp_pair.second < length){
			temp_pair.first=temp_pair.second + 1;
			temp_pair.second=length;
			thread_load_limits.push_back(temp_pair);
		}
		msg("printing limits:\n");
		for(auto it:thread_load_limits){
			msg(boost::lexical_cast<std::string>(it.first) + " " + boost::lexical_cast<std::string>(it.second) + "\n");
		}
		std::vector<std::vector<unsigned>> byte_location_master;
		int count=0;
		for(auto it:thread_load_limits){

			count++;
			msg("count:" + boost::lexical_cast<std::string>(count));
			boost::thread t([&byte_location_master,&it,&index,&is,&length,&count](){

					std::ifstream is;
					std::string file_name= index.getSourceFileName();
					is.open(file_name.c_str(),std::ifstream::binary);
					std::vector<unsigned> byte_location;
					if(is.good()) {
					msg("file is good");
					char c;
					unsigned begin=it.first;
					unsigned end=it.second;
					is.seekg(begin);
					msg("begin " + boost::lexical_cast<std::string>(begin));
					msg("end " + boost::lexical_cast<std::string>(end));
					for(unsigned i = begin; i <=end; i++){
					is.read(&c,1);

					if(c == '\r'){
					std::cout << "tellg:" << is.tellg() << std::endl;
					}
					}
					for(auto it:byte_location){
						std::cout << it << std::endl;

					}
					lock.lock();
					byte_location_master.push_back(byte_location);
					lock.unlock();

					}else {
						msg("file is not good");
						default_throw_function("error opening file " + file_name);
					}
			});

			t.join();
		}

	}else {
		default_throw_function("error opening file " + file_name);
	}
	char c=getchar();
	return 0;
}
