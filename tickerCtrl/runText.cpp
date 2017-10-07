#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "ImageScroller.h"

#define TERM_ERR  "\033[1;31m"
#define TERM_NORM "\033[0m"

volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
    interrupt_received = true;
}

static int usage(const char *progname) {
    fprintf(stderr, "usage: %s <options> [filePath to image.ppm]\n",
                    progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr,
                    "\t-L                        : Large display, in which each chain is 'folded down'\n"
                    "\t                            in the middle in an U-arrangement to get more vertical space.\n"
                    "\t-R <rotation>             : Sets the rotation of matrix. "
                    "Allowed: 0, 90, 180, 270. Default: 0.\n"
                    "\t-t <seconds>              : Run for these number of seconds, then exit.\n");

    rgb_matrix::PrintMatrixFlags(stderr);
    return 1;
}

int main(int argc, char *argv[]) {
    int runtime_seconds = -1;
    int scroll_jumps = 1;
    int scroll_ms = 30;
    int rotation = 0;
    bool large_display = false;

    const char *imageFilePath = NULL;
    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;

    // These are the defaults when no command-line flags are given.
    matrix_options.rows = 32;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;

    // First things first: extract the command line flags that contain
    // relevant matrix options.
    if (!ParseOptionsFromFlags(&argc, &argv, &matrix_options, &runtime_opt)) {
        printf("Failed to parse out matrix options\n");
        return usage(argv[0]);
    }

    int opt;
    while ((opt = getopt(argc, argv, "t:m:LR:")) != -1) {
        switch (opt) {
        case 't':
            runtime_seconds = atoi(optarg);
            break;

        case 'm':
            scroll_ms = atoi(optarg);
            break;

        case 'R':
            rotation = atoi(optarg);
            break;

        case 'L':
            if (matrix_options.chain_length == 1) {
                // If this is still default, force the 64x64 arrangement.
                matrix_options.chain_length = 4;
            }
            large_display = true;
            break;
        }
    }

    if (optind < argc) {
        printf("setting imageFilePath to be %s\n", imageFilePath);
        imageFilePath = argv[optind];
    }

    if (rotation % 90 != 0) {
        fprintf(stderr, TERM_ERR "Rotation %d not allowed! "
                        "Only 0, 90, 180 and 270 are possible.\n" TERM_NORM, rotation);
        return 1;
    }

    RGBMatrix *matrix = CreateMatrixFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL)
        return 1;

    if (large_display) {
        // Mapping the coordinates of a 32x128 display mapped to a square of 64x64.
        // Or any other U-arrangement.
        matrix->ApplyStaticTransformer(UArrangementTransformer(
                                                                         matrix_options.parallel));
    }

    if (rotation > 0) {
        matrix->ApplyStaticTransformer(RotateTransformer(rotation));
    }

    printf("Size: %dx%d. Hardware gpio mapping: %s\n",
                 matrix->width(), matrix->height(), matrix_options.hardware_mapping);

    Canvas *canvas = matrix;

    // The ThreadedCanvasManipulator objects are filling
    // the matrix continuously.
    ThreadedCanvasManipulator *image_gen = NULL;

    if (imageFilePath) {
        ImageScroller *scroller = new ImageScroller(matrix, scroll_jumps, scroll_ms);
        if (!scroller->LoadPPM(imageFilePath)){
            return 1;
        }
        image_gen = scroller;
    } else {
        fprintf(stderr, "scrolling text with scroll_jumps of %d Requires PPM image as parameter\n", scroll_jumps);
        return 1;
    }

    if (image_gen == NULL){
        printf("Image_gen is NULL. Fail\n");
        return 1;
    }
    // Set up an interrupt handler to be able to stop animations while they go
    // on. Note, each demo tests for while (running() && !interrupt_received) {},
    // so they exit as soon as they get a signal.
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    // Image generating demo is crated. Now start the thread.
    image_gen->Start();

    // Now, the image generation runs in the background. We can do arbitrary
    // things here in parallel. In this demo, we're essentially just
    // waiting for one of the conditions to exit.
    if (runtime_seconds > 0) {
        sleep(runtime_seconds);
    } else {
        // The
        printf("Press <CTRL-C> to exit and reset LEDs\n");
        while (!interrupt_received) {
            sleep(1); // Time doesn't really matter. The syscall will be interrupted.
        }
    }

    // Stop image generating thread. The delete triggers
    delete image_gen;
    delete canvas;

    printf("\%s. Exiting.\n", interrupt_received ? "Received CTRL-C" : "Timeout reached");
    return 0;
}