#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include "index.h"

int main(int argc, char** argv) {


	int help_index = 1;

	if(argc == 1 || (argc == 2 && (strncmp(argv[help_index],"--help",6) == 0 || strncmp(argv[help_index],"-help",5) == 0 || strncmp(argv[help_index],"help",4) == 0))){
	
		std::cout << "./index input_file || ./index input_file output_file" << std::endl;
		return 0;	
		
	}
	 
	return 0;
	
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
				std::cout << "\nnew line print\n" << std::endl;
			std::cout << c;
		}
		
	}

return 0;
}

