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
#include <unistd.h>

#include "backgroundLayer.h"
#include "element_change.h"
#include "image.h"
#include "imageLayer.h"
#include "loadpng.h"
#include "key.h"
#include "scrollingLayer.h"
#include "spriteLayer.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

#define NDEBUG

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

    BACKGROUND_LAYER_T bg;
    initBackgroundLayer(&bg, 0x000F, 0);

    SCROLLING_LAYER_T sl;
    initScrollingLayer(&sl, "texture.png", 1);

    IMAGE_LAYER_T spotlight;

    if (loadPng(&(spotlight.image), "spotlight.png") == false)
    {
        fprintf(stderr, "unable to load spotlight\n");
        exit(EXIT_FAILURE);
    }

    createResourceImageLayer(&spotlight, 2);

    SPRITE_LAYER_T sprite;
    initSpriteLayer(&sprite, 12, 1, "sprite.png", 3);

    //---------------------------------------------------------------------

    DISPMANX_DISPLAY_HANDLE_T display
        = vc_dispmanx_display_open(displayNumber);
    assert(display != 0);

    //---------------------------------------------------------------------

    DISPMANX_MODEINFO_T info;
    int result = vc_dispmanx_display_get_info(display, &info);
    assert(result == 0);

    //---------------------------------------------------------------------

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementBackgroundLayer(&bg, display, update);
    addElementScrollingLayerCentered(&sl, &info, display, update);
    addElementImageLayerCentered(&spotlight, &info, display, update);
    addElementSpriteLayerCentered(&sprite, &info, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    int c = 0;

    while (c != 27)
    {
        if (keyPressed(&c))
        {
            c = tolower(c);
            setDirectionScrollingLayer(&sl, c);
        }

        //-----------------------------------------------------------------

        DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
        assert(update != 0);

        updatePositionScrollingLayer(&sl, update);
        updatePositionSpriteLayer(&sprite, update);

        result = vc_dispmanx_update_submit_sync(update);
        assert(result == 0);
    }

    //---------------------------------------------------------------------

    keyboardReset();

    //---------------------------------------------------------------------

    destroyBackgroundLayer(&bg);
    destroyScrollingLayer(&sl);
    destroyImageLayer(&spotlight);
    destroySpriteLayer(&sprite);

    //---------------------------------------------------------------------

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    //---------------------------------------------------------------------

    return 0;
}

