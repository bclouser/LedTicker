#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "graphics.h"
#include "ImageScroller.h"
#include "rpi-rgb-led-matrix/lib/utf8-internal.h"


namespace ImageGen{
	ImageScroller::Image createImage(){
		std::string testString("Ben Says Hello");
		rgb_matrix::Font font;

		if(!font.LoadFont("./fonts/10x20.bdf")){
			std::cout << "Failed to load font" << std::endl;
		}

		int pixelCount = testString.length() * 10 * 20;

		// Color of specified text. Obviously make this dynamic later
		Pixel textColor;
		textColor.red = 128;
		textColor.green = 128;
		textColor.blue = 128;
		unsigned numBitsPerRow = (sizeof(rowbitmap_t)*8);

		printf("Number of bits per row = %d\n", numBitsPerRow);
		// Obviously make these dynamic
		int numRows = 20;
		int currentWidth = 0;

		// create the whole container
		Pixel *pixels = new Pixel [ pixelCount ];

		// calculate the size of our image
		std::string::iterator it = testString.begin();
		while (it != testString.end()) {
		 	const uint32_t codepoint = utf8_next_codepoint(it);
		 	std::cout << "CodePoint = " << codepoint << std::endl;
			const rgb_matrix::Glyph* glyph = font.FindGlyph(codepoint);
			if(glyph == NULL){
				std::cout << "glyph null for char: " << *it << std::endl;
				continue; 
			}
			if(glyph->height != numRows){
				std::cout << "Warning: Glyph mismatch. glyph height = " << glyph->height << "image height = " << numRows <<std::endl;
			}
			std::cout << "numRows = " << numRows << " numColumns = " << glyph->width << std::endl;
			for(int row = 0; row < numRows; ++row){
				printf("bitmap[%d]: = 0x%llX\n", row, glyph->bitmap[row] );
				if(glyph->bitmap[row]){
					std::cout << "Ok so there is something at row: " << row << std::endl;
					for(int column = 0; column < glyph->width; ++column){
						rowbitmap_t columnMask = (rowbitmap_t)1ull<<(numBitsPerRow-column-1);
						printf("trying mask of 0x%llX\n", columnMask);
						if( (rowbitmap_t)glyph->bitmap[row] & columnMask){
							std::cout << "setting pixel for (" << row << "," << column << "). Pixels index: " << std::to_string((currentWidth*row) + column) << std::endl;
							pixels[(currentWidth*row) + column].red = textColor.red;
							pixels[(currentWidth*row) + column].green = textColor.green;
							pixels[(currentWidth*row) + column].blue = textColor.blue;
						}
						else{
							pixels[(currentWidth*row) + column].red = 0x00;
							pixels[(currentWidth*row) + column].green = 0x00;
							pixels[(currentWidth*row) + column].blue = 0x00;
						}
					}
				}
			}

			currentWidth += glyph->width;

			//glyphs.push_back(glyph);
			//std::cout << "glyph height: " << glyph->height << " glyph width: " << glyph->width <<std::endl;
			std::cout << "Current Width =  " << currentWidth << std::endl;
		}
		ImageScroller::Image img;
		img.image=pixels;
		img.width=currentWidth;
		img.height=numRows;
		// Shallow copy ok, we allocated memory for pixels above
		return img;
	}


	void printPretty(ImageScroller::Image& img){
		std::cout << "== Printing Pretty ==" << std::endl;
		std::cout << "Image Height: " << img.height << "\n";
		std::cout << "Image Width: " << img.width << "\n";
		for(int row = 0; row < img.height; ++row){
			std::cout << std::setfill('0') << std::setw(2) << row << " |";
			for(int column=0; column < img.width; ++column){
				int currentIndex = row*img.width + column;
				if(img.image[currentIndex].red || img.image[currentIndex].green || img.image[currentIndex].blue){
					std::cout <<"*"<< "|";
				}else{
					std::cout << " " << "|";
				}
			}
			std::cout <<"\n";
		}
	}
}


