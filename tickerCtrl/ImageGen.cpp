#include <stdlib.h>
#include <iostream>
#include "graphics.h"
#include "rpi-rgb-led-matrix/lib/utf8-internal.h"


namespace ImageGen{

	void doImage(){
		const char* testString = "Ben Says Hello";
		rgb_matrix::Font font;

		if(!font.LoadFont("./fonts/10x20.bdf")){
			std::cout << "Failed to load font" << std::endl;
		}

		while (*testString) {
		 	const uint32_t codepoint = utf8_next_codepoint(testString);
			const rgb_matrix::Glyph* glyph = font.FindGlyph(codepoint);
			std::cout << "glyph height: " << glyph->height << " glyph width: " << glyph->width <<std::endl;
		}
	}
}


