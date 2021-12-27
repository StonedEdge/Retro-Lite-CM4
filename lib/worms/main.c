//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2013 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#define _GNU_SOURCE

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "bcm_host.h"

#include "backgroundLayer.h"
#include "image.h"
#include "key.h"
#include "worms.h"

//-----------------------------------------------------------------------

#define NDEBUG

//-----------------------------------------------------------------------

const char *program = NULL;

//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int opt = 0;

    const char* imageTypeName = "RGBA32";
    VC_IMAGE_TYPE_T imageType = VC_IMAGE_MIN;
    uint16_t  background = 0x0000;
    uint32_t displayNumber = 0;

    program = basename(argv[0]);

    //-------------------------------------------------------------------

    while ((opt = getopt(argc, argv, "b:d:t:")) != -1)
    {
        switch (opt)
        {
        case 'b':

            background = strtol(optarg, NULL, 16);
            break;

        case 'd':

            displayNumber = strtol(optarg, NULL, 10);
            break;

        case 't':

            imageTypeName = optarg;
            break;

        default:

            fprintf(stderr, "Usage: %s \n", program);
            fprintf(stderr, "[-b <RGBA>] [-d <number>] [-t <type>]\n");
            fprintf(stderr, "    -b - set background colour 16 bit RGBA\n");
            fprintf(stderr, "         e.g. 0x000F is opaque black\n");
            fprintf(stderr, "    -d - Raspberry Pi display number\n");
            fprintf(stderr, "    -t - type of image to create\n");
            fprintf(stderr, "         can be one of the following:");
            printImageTypes(stderr,
                            " ",
                            "",
                            IMAGE_TYPES_WITH_ALPHA |
                            IMAGE_TYPES_DIRECT_COLOUR);
            fprintf(stderr, "\n");

            exit(EXIT_FAILURE);
            break;
        }
    }

    //-------------------------------------------------------------------

    IMAGE_TYPE_INFO_T typeInfo;

    if (findImageType(&typeInfo,
                      imageTypeName,
                      IMAGE_TYPES_WITH_ALPHA | IMAGE_TYPES_DIRECT_COLOUR))
    {
        imageType = typeInfo.type;
    }
    else
    {
        fprintf(stderr,
                "%s: unknown image type %s\n",
                program,
                imageTypeName);

        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    bcm_host_init();

    //---------------------------------------------------------------------

    DISPMANX_DISPLAY_HANDLE_T display
        = vc_dispmanx_display_open(displayNumber);
    assert(display != 0);

    //---------------------------------------------------------------------

    DISPMANX_MODEINFO_T info;
    int result = vc_dispmanx_display_get_info(display, &info);
    assert(result == 0);

    //---------------------------------------------------------------------

    BACKGROUND_LAYER_T backgroundLayer;
    initBackgroundLayer(&backgroundLayer, background, 0);

    //---------------------------------------------------------------------

    uint16_t number_of_worms = 36;
    uint16_t worm_length = 25;
    WORMS_T worms;

    initWorms(number_of_worms, worm_length, &worms, imageType, &info);

    //---------------------------------------------------------------------

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementWorms(&worms, display, update);
    addElementBackgroundLayer(&backgroundLayer, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    //---------------------------------------------------------------------

    int c = 0;
    uint32_t frame = 0;

    while (c != 27)
    {
        keyPressed(&c);

        //-----------------------------------------------------------------

        undrawWorms(&worms);
        updateWorms(&worms);
        drawWorms(&worms);
        writeDataWorms(&worms);

        //-----------------------------------------------------------------

        update = vc_dispmanx_update_start(0);
        assert(update != 0);

        changeSourceWorms(&worms, update);

        result = vc_dispmanx_update_submit_sync(update);
        assert(result == 0);

        //-----------------------------------------------------------------

        ++frame;
    }

    //---------------------------------------------------------------------

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    struct timeval diff;
    timersub(&end_time, &start_time, &diff);

    int32_t time_taken = (diff.tv_sec * 1000000) + diff.tv_usec;
    double frames_per_second = (frame * 1000000.0) / time_taken;

    printf("%0.1f frames per second\n", frames_per_second);

    //---------------------------------------------------------------------

    keyboardReset();

    //---------------------------------------------------------------------

    destroyBackgroundLayer(&backgroundLayer);
    destroyWorms(&worms);

    //---------------------------------------------------------------------

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    //---------------------------------------------------------------------

    return 0;
}

