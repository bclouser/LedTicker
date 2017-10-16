#include <iostream>
#include "PPM.h"
#include "BDF.h"


extern void rgb_matrix::doImage();

int main()
{
	/*
	std::cout << "Creating PPM" << std::endl;
	PPM basePpm;
	std::cout << "Calling init" << std::endl;
	basePpm.init("testFile.ppm");


	Pixel dummyPixel = {
		150,
		200,
		222,
		0,
		0,
	};


	std::vector<std::vector<Pixel>> dummyRaster;

	// Add row of 3 pixels
	dummyRaster.push_back({dummyPixel,dummyPixel,dummyPixel});
	
	PPM smallPPM;
	std::cout << "About to add dummyRaster to the smallPPM" << std::endl;
	smallPPM.add(dummyRaster);

	std::cout << "About to print smallPPM" <<std::endl;
	smallPPM.printPretty();


	std::cout << "About to add smallPPM to basePpm" << std::endl;
	// for every pixel in character
	basePpm.add(smallPPM.getRaster());

	basePpm.printPretty();
	basePpm.save();
	*/

	BDF bdf;
	bdf.load("10x20.bdf");

	const uint32_t* bitmap = bdf.getBitMap('A');

	doImage

	return 0;
}


