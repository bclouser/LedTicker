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
			for(int i = 0; i < glyph->width; ++i){
				newImage[i].red = textColor.red;
			}

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


