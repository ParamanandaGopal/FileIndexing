#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include "index.h"
#include <include/tclap/CmdLine.h>

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
        index.set_is_verbose(true);
	
	switch(argc){
		case 2:
			source_file_input = argv[1];
			index.setSourceFileName(source_file_input);

			std::string output_file_name = getOutputFileName(index.getSourceFileName());

			std::cout << "setting output_file to " << output_file_name << std::endl;
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

	//return 0;
	
	std::ifstream is;
	is.open("/Users/gobluelabs/buffer.txt",std::ifstream::binary);
	
	if(is){
		is.seekg(0,is.end);
		int length = is.tellg();
		std::cout << "file length is " << length << " bytes" << std::endl;
		is.seekg(0,is.beg);
		char c;
		for(int i = 0; i < length; i++){
			is.read(&c,1);
			if(c == '\n')
				std::cout << std::endl << is.tellg() << std::endl;
			std::cout << c;
		}
		
	}

return 0;
}

