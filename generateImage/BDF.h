#ifndef _BDF_H_
#define _BDF_H_

#include <stdlib.h>


class BDF {
public:
	bool load(const char* filename);

	// for now we only support ASCII
	const uint32_t* getBitMap(char character);

	unsigned long getHeight(){
		return m_height;
	}
	unsigned long getWidth(){
		return m_width;
	}

private:
	unsigned long m_height=0;
	unsigned long m_width=0;

	// Suppport for widths up to 32 bytes
	uint32_t m_bitMap[256];

};




#endif