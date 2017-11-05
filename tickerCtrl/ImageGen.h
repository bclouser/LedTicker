#ifndef _IMAGE_GEN_H_
#define _IMAGE_GEN_H_ value

#include <string>
#include "ImageScroller.h"

using namespace rgb_matrix;

class ImageGen{
public:
	bool createImageFromEncodedString(std::string& encodedString, ImageScroller::Image& img);
private:
	bool extractTextColor(std::string::iterator& it, Pixel& textColor);

};

#endif