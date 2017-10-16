
#include <stdio.h>
//#include <iterator>
#include <iostream>
#include <fstream>
#include "PPM.h"

/*
		rows	columns
ppmRaster[x][y,y,y,y,y,y,y,y]

so there will likely only be 16 rows and a shit ton of columns
*/


bool PPM::init(const char* filename){
	m_fileName=filename;
	return true;
}

bool PPM::read(const char* fileName){
	// parse header info
	std::ifstream input;
	input.open(fileName, std::ifstream::in);

	if(!input.is_open()){
		std::cout << "Failed to open input file" << std::endl;
		return false;
	}

	

	// Set m_width, m_height, and m_maxColorVal
}

std::vector<std::vector<Pixel>>::const_iterator PPM::begin() const{
	return m_ppmRaster.begin();
}

const std::vector<std::vector<Pixel>>& PPM::getRaster() const{
	return m_ppmRaster;
}


bool PPM::add(const std::vector<std::vector<Pixel>>& rasterIn){
	unsigned height = m_height+rasterIn.size();
	unsigned width = 0;
	if(rasterIn.size() > 0){
		width = m_width+rasterIn[0].size();
	}
	else{
		std::cout << "Oh man, bad ppm passed in" << std::endl;
		return false;
	}

	std::cout << "Ok. incoming raster: height = " << rasterIn.size() << ", width = " << rasterIn[0].size() << std::endl;
	std::cout << "New raster: height = " << height << ", width = " << width << std::endl;

	// Sanity check to see if our system can handle it. 
	if(height < m_ppmRaster.max_size() && (height != m_height)){
		std::cout << "About to resize base height to be " << height << std::endl;
		m_ppmRaster.resize(height);
	}

	// All vectors in a column should be the same size so only need to check the 1st row
	if((width < m_ppmRaster[0].max_size()) && (width != m_width)){
		std::cout << "About to resize base width to be " << width << std::endl;
		// loop through each row and change the width
		for(int row = 0; row < height; ++row){
			std::cout << "Just checking this only happens once" << std::endl;
			m_ppmRaster[row].resize(width);
		}
	}

	std::cout << "And the finale. About to std::copy stuff in" << std::endl;
	// OK, we are all resized. Lets add our content now
	// TODO: unreadable, find a way to make this better
	//std::cout << "begin() = " << rasterIn[0].begin() << "end() = " << 
	std::copy ( rasterIn[0].begin(), rasterIn[0].end(), &m_ppmRaster[0][m_width]);

	m_height = height;
	m_width = width;

	return true;
}

void PPM::printPretty(){
	std::vector<std::vector<Pixel>>::iterator x;
	std::vector<Pixel>::iterator y;

	for(x=m_ppmRaster.begin(); x!= m_ppmRaster.end(); ++x){
		std::cout << x-m_ppmRaster.begin() << ": ";
		for(y=x->begin(); y!= x->end(); ++y){
			std::cout <<"{" << y->red << "," << y->green << "," << y->blue << "}, ";
		}
		std::cout << std::endl;
	}
}

bool PPM::save(){
	std::vector<std::vector<Pixel>>::iterator x;
	std::vector<Pixel>::iterator y;

	std::cout << "Opening file " << m_fileName << std::endl;
	std::ofstream outStream;
	outStream.open(m_fileName, std::ofstream::out|std::ofstream::binary);

	if(!outStream.is_open()){
		std::cout << "dude, file didn't open" << std::endl;
		return false;
	}
	// Header
	outStream << "P6\n" << m_width << "\n" << m_height << "\n" << "255\n";

	std::cout << "about to iterate through and write to file" << std::endl;
	// Raster
	for(x=m_ppmRaster.begin(); x!= m_ppmRaster.end(); ++x){
		//std::cout << x-m_ppmRaster.begin() << ": ";
		for(y=x->begin(); y!= x->end(); ++y){
			//outStream << y->red << "," << y->green << "," << y->blue << "}, ";
			outStream.write((char*)&y->red,1); outStream.write((char*)&y->green,1); outStream.write((char*)&y->blue,1);
		}
	}

	std::cout << "About to close the file." <<std::endl;
	outStream.close();

	//std::cout << "Just saved data to file: " << m_fileName << std::endl;
	return true;
}

// Lookup the colors for a pixel specified by width and height
bool getPixel(Pixel& pixel){

	return true;
}
