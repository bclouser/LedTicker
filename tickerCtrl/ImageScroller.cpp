// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
//
// This code is public domain
// (but note, once linked against the led-matrix library, this is
// covered by the GPL v2)
//
// This is a grab-bag of various demos and not very readable.
#include "ImageScroller.h"

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

using std::min;
using std::max;
using namespace rgb_matrix;
// This is defined in runText.cpp
extern volatile bool interrupt_received;


/******************************************************************************/
/* Public Methods                                                             */
/******************************************************************************/

// _very_ simplified. Can only read binary P6 PPM. Expects newlines in headers
// Not really robust. Use at your own risk :)
// This allows reload of an image while things are running, e.g. you can
// live-update the content.
bool ImageScroller::LoadPPM(const char *filename) {
    m_fileName = filename;
    FILE *f = fopen(filename, "r");
    // check if file exists
    if (f == NULL && access(filename, F_OK) == -1) {
        fprintf(stderr, "File \"%s\" doesn't exist\n", filename);
        return false;
    }
    if (f == NULL) return false;
    char header_buf[256];
    const char *line = ReadLine(f, header_buf, sizeof(header_buf));
#define EXIT_WITH_MSG(m) { fprintf(stderr, "%s: %s |%s", filename, m, line); \
        fclose(f); return false; }
    if (sscanf(line, "P6 ") == EOF)
        EXIT_WITH_MSG("Can only handle P6 as PPM type.");
    line = ReadLine(f, header_buf, sizeof(header_buf));
    int new_width, new_height;
    if (!line || sscanf(line, "%d %d ", &new_width, &new_height) != 2)
        EXIT_WITH_MSG("Width/height expected");
    int value;
    line = ReadLine(f, header_buf, sizeof(header_buf));
    if (!line || sscanf(line, "%d ", &value) != 1 || value != 255)
        EXIT_WITH_MSG("Only 255 for maxval allowed.");
    const size_t pixel_count = new_width * new_height;
    Pixel *new_image = new Pixel [ pixel_count ];
    assert(sizeof(Pixel) == 3);   // we make that assumption.
    if (fread(new_image, sizeof(Pixel), pixel_count, f) != pixel_count) {
        line = "";
        EXIT_WITH_MSG("Not enough pixels read.");
    }
#undef EXIT_WITH_MSG
    fclose(f);
    fprintf(stderr, "Read image '%s' with %dx%d\n", filename,
                    new_width, new_height);
    MutexLock l(&mutex_new_image_);
    new_image_.Delete();  // in case we reload faster than is picked up
    new_image_.image = new_image;
    new_image_.width = new_width;
    new_image_.height = new_height;
    return true;
}

void ImageScroller::printImage(ImageScroller::Image& img){
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

void ImageScroller::printImage(){
    std::cout << "== Printing Pretty ==" << std::endl;
    std::cout << "Image Height: " << current_image_.height << "\n";
    std::cout << "Image Width: " << current_image_.width << "\n";
    for(int row = 0; row < current_image_.height; ++row){
        std::cout << std::setfill('0') << std::setw(2) << row << " |";
        for(int column=0; column < current_image_.width; ++column){
            int currentIndex = row*current_image_.width + column;
            if(current_image_.image[currentIndex].red || current_image_.image[currentIndex].green || current_image_.image[currentIndex].blue){
                std::cout <<"*"<< "|";
            }else{
                std::cout << " " << "|";
            }
        }
        std::cout <<"\n";
    }
}

bool ImageScroller::UpdateImage(ImageScroller::Image& newImage){
    MutexLock l(&mutex_new_image_);
    new_image_.Delete();  // in case we reload faster than is picked up
    new_image_.image = newImage.image;
    new_image_.height = newImage.height;
    new_image_.width = newImage.width;
    std::cout<< "Image height " << new_image_.height << " width " << new_image_.width << std::endl;
    return true;
}


void ImageScroller::Run() {
    bool imageOffScreen = true;
    const int screen_height = offscreen_->height();
    const int screen_width = offscreen_->width();
    // Start negative b/c we want to have our image enter one row at a time
    horizontal_position_ = (-screen_width);
    while (running() && !interrupt_received) {
        {
            MutexLock l(&mutex_new_image_);
            // Dont apply new image until we are done scrolling image off screen
            if (new_image_.IsValid() && imageOffScreen) {
                printf("Loading new image\n");
                current_image_.Delete();
                current_image_ = new_image_;
                new_image_.Reset();
                horizontal_position_ = (-screen_width);
            }
        }
        // Not sure why this is here
        if (!current_image_.IsValid()) {
            usleep(100 * 1000);
            continue;
        }

        imageOffScreen = false;
        //printf("horizontal_position_ = %d\n", horizontal_position_);
        //printf("scroll_jumps_ = %d\n", scroll_jumps_);
        //printf("current_image_.width = %d\n", current_image_.width);
        for (int x = 0; x < screen_width; ++x) {
            // Check for when we have completely scrolled the image off the screen.
            if((horizontal_position_+x) == (current_image_.width + screen_width)) {
                // reset horizontal_position
                horizontal_position_ = (-screen_width);
                imageOffScreen = true;
                break;
            }
            for (int y = 0; y < screen_height; ++y) {
                if(horizontal_position_+x < 0){
                    offscreen_->SetPixel(x, y, 0, 0, 0);
                }
                // when we roll off the edge
                else if (horizontal_position_+x > current_image_.width){
                    offscreen_->SetPixel(x, y, 0, 0, 0);
                }
                else{
                    const Pixel &p = current_image_.getPixel(
                        (horizontal_position_ + x) % current_image_.width, y);
                    offscreen_->SetPixel(x, y, p.red, p.green, p.blue);
                }
            }
        }
        offscreen_ = matrix_->SwapOnVSync(offscreen_);

        horizontal_position_ += scroll_jumps_;

        if (scroll_ms_ <= 0) {

            // No scrolling. We don't need the image anymore.
            current_image_.Delete();
        } else {
            usleep(scroll_ms_ * 1000);
        }
    }
}

/******************************************************************************/
/* Private Methods                                                            */
/******************************************************************************/

// Read line, skip comments.
char* ImageScroller::ReadLine(FILE *f, char *buffer, size_t len) {
    char *result;
    do {
        result = fgets(buffer, len, f);
    } while (result != NULL && result[0] == '#');
    return result;
}