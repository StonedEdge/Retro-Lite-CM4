//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
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
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "image.h"
#include "loadpng.h"
#include "resizeDispmanX.h"
#include "savepng.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

#define NDEBUG

//-------------------------------------------------------------------------

const char *program = NULL;

//-------------------------------------------------------------------------

void usage(void)
{
    fprintf(stderr,
            "Usage: %s -w <width> -h <height> <in.png> <out.png>\n",
            program);
    fprintf(stderr, "    -w - resize to width\n");
    fprintf(stderr, "    -h - resize to height\n");

    exit(EXIT_FAILURE);
}

//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    program = basename(argv[0]);

    int16_t width = 0;
    int16_t height = 0;

    //---------------------------------------------------------------------

    int opt = 0;

    while ((opt = getopt(argc, argv, "w:h:")) != -1)
    {
        switch(opt)
        {
        case 'w':

            width = atoi(optarg);
            break;

        case 'h':

            height = atoi(optarg);
            break;

        default:

            usage();
            break;
        }
    }

    //---------------------------------------------------------------------

    if ((width <= 0) || (height <= 0))
    {
        fprintf(stderr,
                "%s: must specify both width or height\n",
                program);
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    if (optind >= argc)
    {
        usage();
    }

    //---------------------------------------------------------------------

    bcm_host_init();

    //---------------------------------------------------------------------

    IMAGE_T srcImage;

    if (loadPng(&srcImage, argv[optind]) == false)
    {
        fprintf(stderr, "%s: unable to load %s\n", program, argv[optind]);
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    RESIZE_DISPMANX_T resize;

    initResizeDispmanX(&resize,
                       srcImage.type,
                       width,
                       height,
                       srcImage.width,
                       srcImage.height,
                       true);

    //---------------------------------------------------------------------

    IMAGE_T dstImage;

    initImage(&dstImage,
              srcImage.type,
              resize.destinationWidth,
              resize.destinationHeight,
              false);

    //---------------------------------------------------------------------


    resizeDispmanX(&resize, &dstImage, &srcImage);

    //---------------------------------------------------------------------

    if (savePng(&dstImage, argv[optind+1]) == false)
    {
        fprintf(stderr, "%s: unable to save %s\n", program, argv[optind+1]);
    }

    //---------------------------------------------------------------------

    destroyImage(&srcImage);
    destroyImage(&dstImage);

    destroyResizeDispmanX(&resize);

    //---------------------------------------------------------------------

    return 0;
}

