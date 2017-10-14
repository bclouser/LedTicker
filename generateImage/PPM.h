#ifndef _PPM_H_
#define _PPM_H_

#include <vector>

struct Pixel{
	short red;
	short green;
	short blue;
	long x;
	long y;
};

class PPM {
public:
	bool init(const char* filename=NULL);
	bool add(const std::vector<std::vector<Pixel>>& rasterIn);
	bool save();
	unsigned getWidth() const{
		return m_width;
	}
	unsigned getHeight() const{
		return m_height;
	}

	// Give out a pointer to the start of the raster
	std::vector<std::vector<Pixel>>::const_iterator begin() const;
	const std::vector<std::vector<Pixel>>& getRaster() const;

	void printPretty();

private:
	const char* m_fileName = NULL;
	unsigned m_width=0;
	unsigned m_height=0;
	unsigned m_maxColorVal=0;
	std::vector<std::vector<Pixel>> m_ppmRaster;
};



#endif