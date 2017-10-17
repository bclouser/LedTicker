#include <stdlib.h>
#include <iostream>
#include <string>
#include "graphics.h"
#include "ImageScroller.h"
#include "rpi-rgb-led-matrix/lib/utf8-internal.h"


namespace ImageGen{

	Pixel* createImage(){
		std::string testString("Ben Says Hello");
		rgb_matrix::Font font;

		if(!font.LoadFont("./fonts/10x20.bdf")){
			std::cout << "Failed to load font" << std::endl;
		}

		pixelCount = testString.len() * 10 * 20;

		// Color of specified text. Obviously make this dynamic later
		Pixel textColor = {128, 128, 128};

		// Obviously make this dynamic
		numRows = 20;

		// create the whole container
		Pixel *newImage = new Pixel [ pixelCount ];

		// calculate the size of our image
		std::vector<const rgb_matrix::Glyph*> glyphs;
		std::string::iterator it = testString.begin();
		while (it != testString.end()) {
		 	const uint32_t codepoint = utf8_next_codepoint(it);
			const rgb_matrix::Glyph* glyph = font.FindGlyph(codepoint)
			if(glyph == NULL){
				std::cout << "glyph null for char: " << *it << std::endl;
				continue; 
			}
			if(glyph->height != numRows){
				std::cout << "Warning: Glyph mismatch. glyph height = " << glyph->height << "image height = " << numRows <<std::endl;
			}

			for(int row = 0; row < numRows; ++row){
				for(int i = 0; i < glyph->width; ++i){
					if(glyph.bitmap[numRows] & 1<<i){
						newImage[currentWidth*row + i].red = textColor.red;
						newImage[currentWidth*row + i].green = textColor.green;
						newImage[currentWidth*row + i].blue = textColor.blue;
					}
					else{
						newImage[currentWidth*row + i].red = 0x00;
						newImage[currentWidth*row + i].green = 0x00;
						newImage[currentWidth*row + i].blue = 0x00;
					}
				}
			}
			current

			glyphs.push_back(glyph);
			std::cout << "glyph height: " << glyph->height << " glyph width: " << glyph->width <<std::endl;
		}

		
		std::vector<const rgb_matrix::Glyph*>::iterator it=glyph;
		for(std::vector<const rgb_matrix::Glyph*>::iterator it=begin; )
		for(int i = 0; i < pixelCount; ++i){
			newImage[i].red = textColor.red;
		}
		return newImage;
	}
}


