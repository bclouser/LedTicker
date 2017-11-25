#ifndef _IMAGE_SCROLL_H_
#define _IMAGE_SCROLL_H_

#include "led-matrix.h"
#include "threaded-canvas-manipulator.h"
#include "transformer.h"
#include "graphics.h"
#include <iostream>

#include <stdlib.h>
#include <unistd.h>

using namespace rgb_matrix;

struct Pixel {
    Pixel() : red(0), green(0), blue(0){}
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class ImageScroller : public ThreadedCanvasManipulator {
public:
    // Scroll image with "scroll_jumps" pixels every "scroll_ms" milliseconds.
    // If "scroll_ms" is negative, don't do any scrolling.
    ImageScroller(RGBMatrix *m, int scroll_jumps, int scroll_ms = 30)
        : ThreadedCanvasManipulator(m), scroll_jumps_(scroll_jumps),
            scroll_ms_(scroll_ms),
            horizontal_position_(0),
            matrix_(m) {
        offscreen_ = matrix_->CreateFrameCanvas();
    }

    virtual ~ImageScroller() {
        Stop();
        WaitStopped();   // only now it is safe to delete our instance variables.
    }

    struct Image {
        Image() : width(-1), height(-1), image(NULL) {}
        ~Image() { Delete(); }
        void Delete() { delete [] image; Reset(); }
        void Reset() { image = NULL; width = -1; height = -1; }
        inline bool IsValid() { return image && height > 0 && width > 0; }
        const Pixel &getPixel(int x, int y) {
            static Pixel black;
            if (x < 0 || x >= width || y < 0 || y >= height) return black;
            return image[x + width * y];
        }

        int width;
        int height;
        Pixel *image;
    };
    
    bool LoadPPM(const char *filename);
    static bool ParsePPM(const char* filename, ImageScroller::Image& img);
    bool reLoadPPM(){
        return LoadPPM(m_fileName);
    }
    bool UpdateImage(ImageScroller::Image& newImage);
    static void printImageRange(ImageScroller::Image& img, int columnStart, int columnEnd);
    static void printImage(ImageScroller::Image& img);
    void printImage();
    static void saveImage(const char* filename, ImageScroller::Image& img);
    void Run();


    private:


    // Read line, skip comments.
    static char *ReadLine(FILE *f, char *buffer, size_t len);

    const int scroll_jumps_;
    const int scroll_ms_;

    const char* m_fileName;

    // Current image is only manipulated in our thread.
    Image current_image_;

    // New image can be loaded from another thread, then taken over in main thread
    Mutex mutex_new_image_;
    Image new_image_;

    int32_t horizontal_position_;

    RGBMatrix* matrix_;
    FrameCanvas* offscreen_;
};


#endif