#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "index.h"
#include <tclap/CmdLine.h>
boost::mutex lock;
void to_cout(const std::vector<std::string> &v);
void msg(std::string str);

void default_throw_function(std::string str);

void print_map(std::string header, std::map<long,long> map,std::string filename);
void parse_program_options(int argc, char** argv);
int main(int argc, char** argv) {
	std::cout << "parsing program_options" << std::endl;
	parse_program_options(argc,argv);//A parser for options
	//exit(0);
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
	num_threads=6;
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
		if(width <= 10){
			num_threads=1;
			width=length;
		}

		msg("width " + boost::lexical_cast<std::string>(width));
		std::pair<unsigned,unsigned> temp_pair;
		for(unsigned i=0; i< num_threads; i++){
			temp_pair.first=i*width + 1;
			if(i==(num_threads-1))
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
					new boost::thread([&index,&byte_location_master,it,&length,count](){
						std::cout << "job no:" << count << std::endl;
						unsigned local_count=count;
						std::ifstream is;
						std::string file_name= index.getSourceFileName();
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
						lock.lock();
						std::cout << "job:" << local_count << ":";
						std::copy(byte_location.begin(),byte_location.end(),std::ostream_iterator<int>(std::cout," "));

						std::cout << std::endl;

						lock.unlock();

						//msg("\n");
						}} else {
							is.close();
						}
						}
						msg("resource lock " + boost::lexical_cast<std::string>(local_count));
						lock.lock();
						byte_location_master[local_count]=byte_location;
						std::copy(byte_location.begin(),byte_location.end(),std::ostream_iterator<int>(std::cout," "));
						std::cout << std::endl;
						lock.unlock();
						msg("resource unlock " + boost::lexical_cast<std::string>(local_count));
						}else {
							msg("file is not good");
							default_throw_function("error opening file " + file_name);
						}
					}));
			index.add_thread(temp_ptr);
		}
		msg("joining threads");
		index.thread_join();
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
		for(auto it:master_index_){

			std::cout << it.first << " " << it.second << std::endl;
		}
		std::cout << "done printing byte location " << std::endl;
		//	std::string header=file_name  + " " + boost::lexical_cast<std::string>(master_index_.size()) + " " + boost::lexical_cast<std::string>(length) + " " + "\\n";
		//	print_map(header,master_index_,target_file_input);
	}else {
		default_throw_function("error opening file " + file_name);
	}

	//msg("enter any character to quit ");
	//char c=getchar();
	return 0;
}
void parse_program_options(int argc, char** argv){
	try{
		std::cout << "entering program_options argc " << argc << std::endl;
		int age;
		boost::program_options::options_description desc{"Options"};
		desc.add_options()
			("help,h", "Help screen")
			("pi", boost::program_options::value<float>()->implicit_value(3.14f), "Pi")
			("age", boost::program_options::value<int>(&age), "Age")
			("phone", boost::program_options::value<std::vector<std::string>>()->multitoken()->zero_tokens()->composing(), "Phone")
			("unreg", "Unrecognized options");
		boost::program_options::command_line_parser parser{argc, argv};
		parser.options(desc).allow_unregistered().style(
				boost::program_options::command_line_style::default_style |
				boost::program_options::command_line_style::allow_slash_for_short);
		boost::program_options::parsed_options parsed_options = parser.run();
		boost::program_options::variables_map vm;
		boost::program_options::store(parsed_options, vm);
		boost::program_options::notify(vm);
		if (vm.count("help"))
			std::cout << desc << '\n';
		else if (vm.count("age"))
			std::cout << "Age: " << age << '\n';
		else if (vm.count("phone"))
			to_cout(vm["phone"].as<std::vector<std::string>>());
		else if (vm.count("unreg"))
			to_cout(collect_unrecognized(parsed_options.options,
						boost::program_options::exclude_positional));
		else if (vm.count("pi"))
			std::cout << "Pi: " << vm["pi"].as<float>() << '\n';
	}
	catch (const boost::program_options::error &ex)
	{
		std::cerr << ex.what() << '\n';
	}

	std::cout << "done with program options" << '\n';
}
void to_cout(const std::vector<std::string> &v)
{
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>{
			std::cout, "\n"});
}
void msg(std::string str) { std::cout << str << std::endl;}
void default_throw_function(std::string str){
	throw str;
}
void print_map(std::string header, std::map<long,long> map,std::string filename){
	std::ofstream file(filename.c_str());
	if(file.good()){
		file << header << std::endl;
		for(auto it:map){
			file << it.first << " " << it.second << std::endl;
		}
	} else {
		default_throw_function("can not open file " + filename + " for writting");
	}
}
