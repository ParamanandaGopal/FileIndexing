#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "index.h"
#include <tclap/CmdLine.h>

void parse_program_options(int argc, char** argv);
int main(int argc, char** argv) {
	std::cout << "parsing program_options" << std::endl;
	parse_program_options(argc,argv);//A parser for options
	int help_index = 1;
	std::string source_file_input;
	std::string target_file_input;
	source_file_input=argv[1];
	int num_threads = 1;
	if(argc == 1 || (argc == 2 && (strncmp(argv[help_index],"--help",6) == 0 || strncmp(argv[help_index],"-help",5) == 0 || strncmp(argv[help_index],"help",4) == 0))){
		std::cout << "./index input_file || ./index input_file output_file || || ./index input_file output_file input_file || ./index input_file output_file number_of_threads(default 1) || ./index input_file output_file number_of_threads(default 1) buffer_size(~1MB)" << std::endl;
		return 0;
	}
	Index index;
	index.setSourceFileName(source_file_input);
	Index::msg("SETTING VERBOSE MODE");
	index.setIsVerbose(true);
	Index::msg("switching to case " + boost::lexical_cast<std::string>(argc));
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
	index.createTargetFileName();
	std::cout << "target filename:" << index.getTargetFileName() << std::endl;
	index.createIndex();
	Index::msg("enter any character to quit ");
	char c=getchar();
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
			Index::to_cout(vm["phone"].as<std::vector<std::string>>());
		else if (vm.count("unreg"))
			Index::to_cout(collect_unrecognized(parsed_options.options,
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

