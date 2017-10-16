#include "BDF.h"

bool BDF::load(const char* filename){
	// parse header info
	std::ifstream input;
	input.open(filename, std::ifstream::in);

	if(!input.is_open()){
		std::cout << "Failed to open input file" << std::endl;
		return false;
	}


	input.
}
	
// for now we only support ASCII
const uint32_t* BDF::getBitMap(char character){
	return NULL;
}