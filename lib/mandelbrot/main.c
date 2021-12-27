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
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "bcm_host.h"

#include "backgroundLayer.h"
#include "font.h"
#include "imageGraphics.h"
#include "imageLayer.h"
#include "info.h"
#include "key.h"
#include "mandelbrot.h"
#include "savepng.h"

//-------------------------------------------------------------------------

#define NDEBUG

//-------------------------------------------------------------------------

bool
zoom(
    IMAGE_LAYER_T *zoomLayer,
    IMAGE_LAYER_T *infoLayer,
    MANDELBROT_COORDS_T *coords)
{
    bool changed = false;

    static RGBA8_T maskColour = { 127, 127, 127, 127 };
    static RGBA8_T clearColour = { 0, 0, 0, 0 };

    int32_t width = zoomLayer->image.width / 4;
    int32_t height = zoomLayer->image.height / 4;
    int32_t x = (zoomLayer->image.width - width) / 2;
    int32_t y = (zoomLayer->image.height - height) / 2;

    static int32_t steps[] = { 1, 2, 5, 10, 20, 50 };
    static int32_t numberOfSteps = sizeof(steps) / sizeof(steps[0]);
    static int32_t stepIndex = 0;
    int32_t step = steps[stepIndex];

    clearImageRGB(&(zoomLayer->image), &maskColour);
    imageBoxFilledRGB(&(zoomLayer->image),
                      x,
                      y,
                      x + width - 1,
                      y + height - 1,
                      &clearColour);
    changeSourceAndUpdateImageLayer(zoomLayer);

    zoomInfo(infoLayer, steps, numberOfSteps, stepIndex);

    int c = 0;
    while ((c != 27) && (changed == false))
    {
        usleep(100000);

        if (keyPressed(&c))
        {
            c = tolower(c);

            bool update = false;

            switch (c)
            {
            case 10:

                changed = true;
                break;

            case 'a':

                if ((x - step) >= 0)
                {
                    x -= step;
                    update = true;
                }
                break;

            case 'd':

                if ((x + width + step) <= zoomLayer->image.width)
                {
                    x += step;
                    update = true;
                }
                break;

            case 'w':

                if ((y - step) >= 0)
                {
                    y -= step;
                    update = true;
                }
                break;

            case 's':

                if ((y + height + step) <= zoomLayer->image.height)
                {
                    y += step;
                    update = true;
                }
                break;

            case ']':

                if (stepIndex < (numberOfSteps - 1))
                {
                    step = steps[++stepIndex];
                    zoomInfo(infoLayer, steps, numberOfSteps, stepIndex);
                }

                break;

            case '[':

                if (stepIndex > 0)
                {
                    step = steps[--stepIndex];
                    zoomInfo(infoLayer, steps, numberOfSteps, stepIndex);
                }

                break;
            }

            if (update)
            {
                clearImageRGB(&(zoomLayer->image), &maskColour);
                imageBoxFilledRGB(&(zoomLayer->image),
                                  x,
                                  y,
                                  x + width - 1,
                                  y + height - 1,
                                  &clearColour);
                changeSourceAndUpdateImageLayer(zoomLayer);
            }
        }
    }

    clearImageRGB(&(zoomLayer->image), &clearColour);
    changeSourceAndUpdateImageLayer(zoomLayer);

    if (changed)
    {
        coords->x0 += (coords->side * x) / zoomLayer->image.width;
        coords->y0 += (coords->side * y) / zoomLayer->image.height;
        coords->side *=  (double)width / zoomLayer->image.width;
    }

    return changed;
}

//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    uint32_t displayNumber = 0;

    //-------------------------------------------------------------------

    int opt;

    while ((opt = getopt(argc, argv, "d:")) != -1)
    {
        switch (opt)
        {
        case 'd':

            displayNumber = atoi(optarg);
            break;

        default:

            fprintf(stderr, "Usage: %s [-d <number>]\n", basename(argv[0]));
            fprintf(stderr, "    -d - Raspberry Pi display number\n");
            exit(EXIT_FAILURE);
            break;
        }
    }

    //-------------------------------------------------------------------

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

    int32_t infoLayerWidth = 140;
    int32_t infoLayerHeight = 232;

    int32_t mandelbrotLayerSize = info.height;

    if ((infoLayerWidth + mandelbrotLayerSize) > info.width)
    {
        mandelbrotLayerSize = info.width - infoLayerWidth;
    }

    //---------------------------------------------------------------------

    BACKGROUND_LAYER_T bg;
    initBackgroundLayer(&bg, 0x000F, 0);

    IMAGE_LAYER_T mandelbrotLayer;
    initImageLayer(&mandelbrotLayer,
                   mandelbrotLayerSize,
                   mandelbrotLayerSize,
                   VC_IMAGE_RGB888);
    createResourceImageLayer(&mandelbrotLayer, 1);

    IMAGE_LAYER_T zoomLayer;
    initImageLayer(&zoomLayer,
                   mandelbrotLayer.image.width,
                   mandelbrotLayer.image.height,
                   VC_IMAGE_RGBA16);
    createResourceImageLayer(&zoomLayer, 10);

    IMAGE_LAYER_T infoLayer;
    initImageLayer(&infoLayer,
                   infoLayerWidth,
                   infoLayerHeight,
                   VC_IMAGE_RGBA16);
    createResourceImageLayer(&infoLayer, 2);

    //---------------------------------------------------------------------

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);

    //---------------------------------------------------------------------

    int32_t combinedWidth = mandelbrotLayer.image.width
                          + infoLayer.image.width;

    int32_t offset = (info.width - combinedWidth) / 2;

    addElementBackgroundLayer(&bg, display, update);

    addElementImageLayerOffset(&mandelbrotLayer,
                               offset,
                               0,
                               display,
                               update);

    addElementImageLayerOffset(&zoomLayer, offset, 0, display, update);

    offset += mandelbrotLayer.image.width;

    addElementImageLayerOffset(&infoLayer, offset, 0, display, update);

    //---------------------------------------------------------------------

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    MANDELBROT_T mandelbrot;
    newMandelbrot(&mandelbrot, &mandelbrotLayer);

    //---------------------------------------------------------------------

    MANDELBROT_COORDS_T coords = { -2.0, -1.5, 3.0 };

    calculatingInfo(&infoLayer, mandelbrot.numberOfThreads);
    mandelbrotImage(&mandelbrot, &coords);
    mandelbrotInfo(&infoLayer);

    //---------------------------------------------------------------------

    int c = 0;
    while (c != 27)
    {
        if (keyPressed(&c))
        {
            c = tolower(c);

            switch (c)
            {
            case 's':
            {
                time_t now;
                time(&now);

                struct tm *tm = localtime(&now);

                char filename[128];
                snprintf(filename,
                         sizeof(filename),
                         "mandelbrot_%4d%02d%02d_%2d%02d%02d.png",
                         tm->tm_year + 1900,
                         tm->tm_mon,
                         tm->tm_mday,
                         tm->tm_hour,
                         tm->tm_min,
                         tm->tm_sec);

                savePng(&(mandelbrotLayer.image), filename);
                break;
            }
            case 'z':

                if (zoom(&zoomLayer, &infoLayer, &coords))
                {
                    calculatingInfo(&infoLayer, mandelbrot.numberOfThreads);
                    mandelbrotImage(&mandelbrot, &coords);
                }

                mandelbrotInfo(&infoLayer);

                break;
            }
        }
        else
        {
            usleep(100000);
        }
    }

    //---------------------------------------------------------------------

    keyboardReset();

    //---------------------------------------------------------------------

    destroyMandelbrot(&mandelbrot);
    destroyBackgroundLayer(&bg);
    destroyImageLayer(&mandelbrotLayer);
    destroyImageLayer(&zoomLayer);
    destroyImageLayer(&infoLayer);

    //---------------------------------------------------------------------

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    //---------------------------------------------------------------------

    return 0;
}

