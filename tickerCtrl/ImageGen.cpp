
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ImageGen.h"
#include "graphics.h"
#include "rpi-rgb-led-matrix/lib/utf8-internal.h"


using namespace rgb_matrix;

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
    unsigned numBitsPerRow = (sizeof(rowbitmap_t)*8);
    // Obviously make these dynamic
    int numRows = 15;
    int numPixPerRow = 0;
    int currentWidth = 0;

    std::string::iterator it = encodedString.begin();
    
    // We loop through the whole thing first to determine the actual row length
    // Its annoying that we run the same loop twice :(
	while(it!=encodedString.end()){
    	if((*it == '&') && (*(it+1) == 'e')){
    		std::cout << "Found an emoji" << std::endl;
    	    // currently we just skip emojis
    	    // &eRANR = 6 chars
    	    it+=6;
    	    // We would need to go lookup how wide this emoji is and add it to row width
    	    // numPixPerRow+=emoji.width;
    	}
    	// '&c' is a color sequence
    	else if((*it == '&') && (*(it+1) == 'c')){
    		std::cout << "Found a color sequence" << std::endl;
    	    // updates the current color and  advances string index to next item
    	    if(!extractTextColor(it, currentTextColor)){
    	        std::cout << "Bad color string specified" << std::endl;
    	        return false;
    	    }
    	    // Colors DONT take up pixel space, so the row width doesn't change
    	}
    	else{
    		// Regular character. Update by width of character. 9 b/c 9x15 font
    		numPixPerRow += 9;
    		++it;
    	}
    }

    std::cout << "NumPixPerRow found to be: " << numPixPerRow << std::endl;
 
    int pixelCount = numPixPerRow * numRows;
    std::cout << "That makes the whole pixelCount: " << pixelCount << std::endl;
   
    // now we can create the whole container b/c we know proper dimensions
    Pixel *pixels = new Pixel [ pixelCount ];

    // We bomb through the whole string, one character at a time
    it = encodedString.begin();
    while (it != encodedString.end()) {

        // First figure out what kind of item we are dealing with here (char, emoji, or color)
        // '&e' is an Emoji
        if((*it == '&') && (*(it+1) == 'e')){
            // currently we just skip emojis
            // &eRANR = 6 chars
            it+=6;
            continue;
        }
        // '&c' is a color sequence
        else if((*it == '&') && (*(it+1) == 'c')){
            // updates the current color and  advances string index to next item
            if(!extractTextColor(it, currentTextColor)){
                std::cout << "Bad color string specified" << std::endl;
                return false;
            }
            continue;
        }
        // else regular ole character


        // Codepoint is utf thing. Basically think of it like a character
        const uint32_t codepoint = utf8_next_codepoint(it); // this advances the iterator
        const rgb_matrix::Glyph* glyph = font.FindGlyph(codepoint);
        if(glyph == NULL){
            std::cout << "glyph null for char: " << *it << std::endl;
            continue; 
        }
        if(glyph->height != numRows){
            std::cout << "Warning: Glyph mismatch. glyph height = " << glyph->height << "image height = " << numRows <<std::endl;
        }
        for(int row = 0; row < numRows; ++row){
            if(glyph->bitmap[row]){
                for(int column = 0; column < glyph->width; ++column){
                    // Offset of each row in the whole image plus this char's index and finally the item's column offset
                    unsigned long currentIndex = numPixPerRow*row + currentWidth + column;
                    rowbitmap_t columnMask = (rowbitmap_t)1ull<<(numBitsPerRow-column-1);
                    if( (rowbitmap_t)glyph->bitmap[row] & columnMask){
                        pixels[currentIndex].red = currentTextColor.red;
                        pixels[currentIndex].green = currentTextColor.green;
                        pixels[currentIndex].blue = currentTextColor.blue;
                    }
                    else{
                        pixels[currentIndex].red = 0x00;
                        pixels[currentIndex].green = 0x00;
                        pixels[currentIndex].blue = 0x00;
                    }
                }
            }
        }

        currentWidth += glyph->width;

        //glyphs.push_back(glyph);
        //std::cout << "glyph height: " << glyph->height << " glyph width: " << glyph->width <<std::endl;
        //std::cout << "Current Width =  " << currentWidth << std::endl;
    }
    img.image=pixels;
    img.width=currentWidth;
    img.height=numRows;

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