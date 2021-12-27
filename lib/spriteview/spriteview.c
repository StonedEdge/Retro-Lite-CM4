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
#include "key.h"
#include "spriteLayer.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

#define NDEBUG

//-------------------------------------------------------------------------

const char *program = NULL;

//-------------------------------------------------------------------------

void usage(void)
{
    fprintf(stderr, "Usage: %s ", program);
    fprintf(stderr, "[-b <RGBA>] [-c <columns>] [-d <number>] ");
    fprintf(stderr, "[-l <layer] [-r <row>] <file.png>\n");
    fprintf(stderr, "    -b - set background colour 16 bit RGBA\n");
    fprintf(stderr, "         e.g. 0x000F is opaque black\n");
    fprintf(stderr, "    -c - number of columns in sprite\n");
    fprintf(stderr, "    -d - Raspberry Pi display number\n");
    fprintf(stderr, "    -i - Interval between changes in ms\n");
    fprintf(stderr, "    -l - DispmanX layer number\n");
    fprintf(stderr, "    -n - non-interactive mode\n");
    fprintf(stderr, "    -r - number of rows in sprite\n");
    fprintf(stderr, "    -t - Timeout in ms\n");
    fprintf(stderr, "    -x - offset (pixels from the left)\n");
    fprintf(stderr, "    -y - offset (pixels from the top)\n");

    exit(EXIT_FAILURE);
}

//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    uint16_t background = 0x000F;
    int32_t layer = 1;
    uint32_t displayNumber = 0;
    uint32_t interval = 0;
    uint32_t timeout = 0;
    int32_t xOffset = 0;
    int32_t yOffset = 0;
    bool interactive = true;
    bool xOffsetSet = false;
    bool yOffsetSet = false;
    int columns = 1;
    int rows = 1;
    const char *file = NULL;

    program = basename(argv[0]);

    //---------------------------------------------------------------------

    int opt = 0;

    while ((opt = getopt(argc, argv, "b:c:d:i:l:r:t:x:y:n")) != -1)
    {
        switch(opt)
        {
        case 'b':

            background = strtol(optarg, NULL, 16);
            break;

        case 'c':

            columns = atoi(optarg);
            break;

        case 'd':

            displayNumber = atoi(optarg);
            break;

        case 'i':

            interval = atoi(optarg);
            break;

        case 'l':

            layer = atoi(optarg);
            break;

        case 'n':

            interactive = false;
            break;

        case 'r':

            rows = atoi(optarg);
            break;

        case 't':

            timeout = atoi(optarg);
            break;

        case 'x':

            xOffset = strtol(optarg, NULL, 10);
            xOffsetSet = true;
            break;

        case 'y':

            yOffset = strtol(optarg, NULL, 10);
            yOffsetSet = true;
            break;

        default:

            usage();
            break;
        }
    }

    //---------------------------------------------------------------------

    if (optind >= argc)
    {
        usage();
    }

    file = argv[optind];

    //---------------------------------------------------------------------

    bcm_host_init();

    //---------------------------------------------------------------------

    BACKGROUND_LAYER_T bg;
    initBackgroundLayer(&bg, background, 0);

    SPRITE_LAYER_T sprite;
    initSpriteLayer(&sprite, columns, rows, file, layer);

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

    if (background > 0)
    {
        addElementBackgroundLayer(&bg, display, update);
    }

    if (xOffsetSet == false)
    {
        xOffset = (info.width - sprite.width) / 2;
    }

    if (yOffsetSet == false)
    {
        yOffset = (info.height - sprite.height) / 2;
    }

    addElementSpriteLayerOffset(&sprite,
                                xOffset,
                                yOffset,
                                display,
                                update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    int c = 0;
    bool paused = false;
    bool step = false;
    bool run = true;

    uint32_t currentTime = 0;

    while (run)
    {
        if (interactive && keyPressed(&c))
        {
            c = tolower(c);

            switch (c)
            {
            case 'p':

                paused = !paused;
                break;

            case ' ':

                if (paused)
                {
                    step = true;
                }
                break;
            
            case 27:
                run = false;
                break;
            }

        }

        //-----------------------------------------------------------------

        if ((paused == false) || step)
        {
            DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
            assert(update != 0);

            updatePositionSpriteLayer(&sprite, update);

            result = vc_dispmanx_update_submit_sync(update);
            assert(result == 0);

            step = false;
        }

        if (!interactive)
        {
            usleep(interval*1000);
            currentTime += interval;
            if (timeout != 0 && currentTime >= timeout)
                run = false;
        }
    }

    //---------------------------------------------------------------------

    keyboardReset();

    //---------------------------------------------------------------------

    destroyBackgroundLayer(&bg);
    destroySpriteLayer(&sprite);

    //---------------------------------------------------------------------

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    //---------------------------------------------------------------------

    return 0;
}

