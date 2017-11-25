
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <map>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ImageGen.h"
#include "graphics.h"
#include "rpi-rgb-led-matrix/lib/utf8-internal.h"

#define MAX_NUM_PIXELS 60000

const std::string emojiDir("emojis");
const unsigned screenHeightPix = 16;
std::map<std::string, ImageScroller::Image> emojiLookupMap;

using namespace rgb_matrix;

bool ImageGen::init(){

	// setup our emoji map
	if(!getFilesInFolder(emojiDir, m_emojiExistsMap)){
		std::cout << "Failed to populate emojis map" << std::endl;
		return false;
	}

	m_initialized = true;
	return true;
}

bool ImageGen::getFilesInFolder(const std::string& dir, std::unordered_map<std::string, bool>& filesMap){
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return false;
    }

    while ((dirp = readdir(dp)) != NULL) {
    	std::cout << "Found file " << dirp->d_name << std::endl;
        filesMap[std::string(dirp->d_name)] = true;
    }
    closedir(dp);
    return true;
}

std::string parseEmojiString(const std::string& superSet){
	// We haven't yet parsed in/saved this emoji, so lets do that now.
	const std::string regularChars("abcdefghijklmnopqrstuvwxyz\
			ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_");

	std::string::const_iterator it = superSet.begin();

	if(*it != ':'){
		return std::string("");
	}
	// TODO: we should probably put a limit on how long the emoji string can be.
	bool foundEndingSemicolon = false;
	for(it=superSet.begin() + 1; it != superSet.end(); ++it){
		// If we find the trailing colon, we should be good.
		if( (*it == ':') && ((it - superSet.begin()) > 1)){
			foundEndingSemicolon = true;
			break;
		}
		// Any spaces, or special characters are an immediate bust
		if(regularChars.find(*it) == std::string::npos){
			return std::string("");
		}
	}
	if(foundEndingSemicolon){
		// Return string WITHOUT colons
		return std::string(superSet.begin()+1, it);
	}
	
	return std::string("");
}

// Function will lookup emoji or attempt to parse it in
bool ImageGen::getEmoji(std::string emojiFileName, ImageScroller::Image& outImg){
	// Check if we have already saved the emoji in memory
	// if(emojiLookupMap.count(emojiCode)){
	// 	std::cout << "Oh ok, emoji already exists in map" << std::endl;
	// 	// This needs to be a copy
	// 	outImg = emojiLookupMap[emojiCode];
	// 	//outImg.width = img.width;
	// 	//outImg.height = img.height;
	// 	//outImg.image = img.image;
	// 	return true;
	// }

	std::string emojiPath(emojiDir + std::string("/") + emojiFileName);
	std::cout << "emojiPath = " << emojiPath << std::endl;

	if(!ImageScroller::ParsePPM(emojiPath.c_str(), outImg)){
		std::cout << "Failed to parse ppm emoji image" <<std::endl;
		return false;
	}

	if(outImg.image == NULL){
		std::cout << "Very Bad, emoji image read in is NULL" << std::endl;
		return false;
	}

	return true;
}

bool getCharacter(std::string::iterator& it, const rgb_matrix::Font& font, 
					const Pixel& color, ImageScroller::Image& imageBlock){
	// Codepoint is utf thing. Basically think of it like a character
	uint32_t codepoint = utf8_next_codepoint(it); // this advances the iterator
	const rgb_matrix::Glyph* glyph = font.FindGlyph(codepoint);
	if(glyph == NULL){
		std::cout << "Failed to find glyph for character " << *it << std::endl;
		it--; // undo advance done by utf8_next_codepoint()
		return false;
	}

	//std::cout << *it << ": Creating image with width = " << glyph->width << " and height " << glyph->height << std::endl;
	try {
		imageBlock.image = new Pixel [ glyph->width * glyph->height ];
  	} catch(const std::exception& e) {
    	// = std::current_exception();
    	std::cout << "exception caught: " << e.what() << std::endl;
    	it--; // undo advance done by utf8_next_codepoint()
    	return false;
  	}

	if(imageBlock.image == NULL){
        std::cout << "Failed to allocate memory for imageBlock: " << *it << std::endl;
        it--; // undo advance done by utf8_next_codepoint()
        return false;
    }

	// Convert glyph into image... shouldnt be hard
	for(int row = 0; row < glyph->height; ++row){
        for(int column = 0; column < glyph->width; ++column){
            // Offset of each row in the image plus the individual column offset
            unsigned numBitsPerRow = (sizeof(rowbitmap_t)*8);
            unsigned long currentIndex = glyph->width*row  + column;
            rowbitmap_t columnMask = (rowbitmap_t)1ull<<(numBitsPerRow-column-1);
            if( (rowbitmap_t)glyph->bitmap[row] & columnMask){
       			imageBlock.image[currentIndex].red = color.red;
       			imageBlock.image[currentIndex].green = color.green;
       			imageBlock.image[currentIndex].blue = color.blue;
            }
            else{
       			imageBlock.image[currentIndex].red = 0x00;
       			imageBlock.image[currentIndex].green = 0x00;
       			imageBlock.image[currentIndex].blue = 0x00;
            }
        }
	}
	imageBlock.width = glyph->width;
	imageBlock.height = glyph->height;
	return true;
}


bool ImageGen::determineImageWidth(const std::string& inString, const rgb_matrix::Font& font, unsigned& pixelWidth){
	std::string::const_iterator it = inString.begin();
	pixelWidth = 0;
	unsigned numPixPerRow = 0;
	while(it!=inString.end()){
    	if(*it == ':') {
    		std::string emojiStr = parseEmojiString(std::string(it, inString.end()));

    	    if(emojiStr.size() > 0){
    	    	if(m_emojiExistsMap.find(emojiStr+std::string(".ppm")) != m_emojiExistsMap.end()){
    	    		// parsed and found. Add emoji width
    	    		numPixPerRow+=16;
    	    	}
    	    	else{
    	    		std::cout << "Couldnt find emoji file for " << emojiStr << std::endl;
    	    	}
	    		// Add size of emojiString + 2 for the leading and trailing colons
	    		it+=emojiStr.size() + 2;
    	    }
    	    else{
    	    	// Treat it as a regular character
    	    	uint32_t characterWidth = font.CharacterWidth(utf8_next_codepoint(it));
    	    	//std::cout << "Character has width of " << characterWidth << std::endl;
    	    	numPixPerRow += characterWidth;
    	    }
     	}
    	// '&c' is a color sequence
    	else if(((*it) == '&') && ((*(it+1)) == 'c')){
    	    // updates the current color and  advances string index to next item
    	    it+=8;
    	    // Colors DONT take up pixel space, so the row width doesn't change
    	}
    	// Regular character
    	else{
    		// Regular character. Update by width of character. 9 b/c 9x15 font
    		uint32_t characterWidth = font.CharacterWidth(utf8_next_codepoint(it));
    		//std::cout << "Character has width of " << characterWidth << std::endl;
    		numPixPerRow += characterWidth;
    	}
    }
    pixelWidth = numPixPerRow;
    return true;
}

bool ImageGen::createImageFromEncodedString(std::string& encodedString, ImageScroller::Image& img){
    rgb_matrix::Font font;

    // TODO, really should do this elsewhere... Maybe a load font method
    if(!font.LoadFont("./fonts/9x15.bdf")){
        std::cout << "Failed to load font" << std::endl;
    }

    // Color of specified text
    Pixel currentTextColor;
    currentTextColor.red = 0xFF;
    currentTextColor.green = 0xFF;
    currentTextColor.blue = 0xFF;

    // Obviously make these dynamic
    const unsigned numRows = 16;
    unsigned numPixPerRow = 0;
    unsigned currentWidth = 0;


    if(!determineImageWidth(encodedString, font, numPixPerRow)){
    	std::cout << "Failed to get width of whole image" << std::endl;
    	return false;
    }

    std::cout << "NumPixPerRow found to be: " << numPixPerRow << std::endl;
    unsigned pixelCount = numPixPerRow * numRows;
    std::cout << "That makes the whole pixelCount: " << pixelCount << std::endl;
    // now we can create the whole container b/c we know proper dimensions

    Pixel *pixels = new Pixel[MAX_NUM_PIXELS];

    //std::cout << "\n\n=========================================================\n\n\n";

    // We bomb through the whole string, one character at a time
    std::string::iterator it = encodedString.begin();
    while (it != encodedString.end()) {
    	{ // Using this scope to correctly handle the imageBlock mem management
	    	ImageScroller::Image imageBlock;
	    	//std::cout << "Processing char: " << *it << " nextChar: " << *(it+1) << std::endl;

	        // First figure out what kind of item we are dealing with here (color, emoji, or char)
	        // '&c' is a color sequence
	        if(((*it) == '&') && ((*(it+1)) == 'c')){
	        	std::cout << "start of color string" << std::endl;
	            // updates the current color and  advances string index to next item
	            if(!extractTextColor(it, currentTextColor)){
	                std::cout << "Bad color string specified" << std::endl;
	                return false;
	            }
	            // no image to process. We can just start this loop over.
	            continue;
	        }
	        // '&e' is an Emoji
	        else if(*it == ':'){
	        	std::string emojiStr = parseEmojiString(std::string(it, encodedString.end()));
	        	std::string emojiFile = emojiStr + std::string(".ppm");
	        	std::cout << "emojiFile = " << emojiFile << std::endl;

	        	if(emojiStr.size()>0){
		        	// +2 for the leading and trailing colons
					it+=emojiStr.size()+2;

	        		if(m_emojiExistsMap.find(emojiFile)!=m_emojiExistsMap.end()){
			            // parse emoji as an image
			            if(!getEmoji(emojiFile, imageBlock)){
			            	std::cout << "Failed to create emoji image" <<std::endl;
			            	continue;
			        	}
		        	}
		        	else{
		        		// file not in map, it doesn't exist on this system
		        		continue;
		        	}
    	        }
		        else{
	            	// If it wasn't an emoji, we should parse it as a character
	    			if(!getCharacter(it, font, currentTextColor, imageBlock)){
		        		std::cout << "Failed to get character" <<std::endl;
		        		continue;
		        	}
		        }
	        }
	        // Regular ole character
	        else{
	        	if(!getCharacter(it, font, currentTextColor, imageBlock)){
	        		std::cout << "Failed to get character" <<std::endl;
	        		continue;
	        	}
	        }

	        std::cout << "updating imageBlock: width = " << imageBlock.width << " height " << imageBlock.height << std::endl;
	      
	        // TODO: Make this suck less. sCheck that we have enough room in our buffer for the next image segment
	        if(((currentWidth + imageBlock.width) * 16) > MAX_NUM_PIXELS){
	        	std::cout << "Out of space in our buffer. Clipping the image here" << std::endl;
	        	break;
	        }

	        // Copy data into master image container
	        for(int row = 0; row < imageBlock.height; ++row){
	        	for(int column = 0; column < imageBlock.width; ++column){
	        		pixels[row*numPixPerRow + currentWidth + column] = imageBlock.image[row*imageBlock.width + column];
	        	}
	        }

	        currentWidth += imageBlock.width;
    	}
    }
    // internal check
    if(currentWidth != numPixPerRow){
    	std::cout << "INTERNAL ERROR, width of image calculated incorrectly" << std::endl;
    	std::cout << "Expected width = " << numPixPerRow << ". Width of created image " << currentWidth <<std::endl;
    	return false;
    }

    img.image=pixels;
    img.width=currentWidth;
    img.height=numRows;

    std::cout << "complete image width = " << img.width << ", height = " << img.height << ", pixCount: " <<img.height*img.width << std::endl;

    //ImageScroller::saveImage("/opt/tickerStuff/scrollingImage.raw", img);

    // used for veryifying end of image wasn't bad
    //ImageScroller::printImageRange(img, img.width-60, img.width);

    return true;
}


///////////////////////////// Private Guys
bool ImageGen::extractTextColor(std::string::iterator& it, Pixel& textColor){
	//&cFFFFFF = 8 chars
	unsigned color;
	std::string temp(it+2, it+8);
	std::stringstream stream(temp);
	stream >> std::hex >> color;

	textColor.red = (color >> 16) & 0xFF;
	textColor.green = (color >> 8) & 0xFF;
	textColor.blue = color & 0xFF;

	it += 8;
	return true;
}