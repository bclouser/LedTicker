#ifndef _IMAGE_GEN_H_
#define _IMAGE_GEN_H_ value

#include <string>
#include <unordered_map>
#include "ImageScroller.h"

using namespace rgb_matrix;

class ImageGen{
public:
	bool init();
	bool createImageFromEncodedString(std::string& encodedString, ImageScroller::Image& img);
private:
	bool extractTextColor(std::string::iterator& it, Pixel& textColor);
	bool determineImageWidth(const std::string& inString, const rgb_matrix::Font& font, unsigned& pixelWidth);
	bool getFilesInFolder(const std::string& dir, std::unordered_map<std::string, bool>& filesMap);
	bool getEmoji(std::string emojiFileName, ImageScroller::Image& outImg);

	bool m_initialized = false;
	std::unordered_map<std::string, bool> m_emojiExistsMap;

};

#endif