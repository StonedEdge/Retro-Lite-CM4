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

#include <inttypes.h>
#include <stdint.h>

#include "font.h"
#include "imageGraphics.h"
#include "imageKey.h"
#include "imageLayer.h"
#include "info.h"

//-------------------------------------------------------------------------

#define INFO_LEFT_PADDING 4
#define INFO_TOP_PADDING 4

//-------------------------------------------------------------------------

void
lifeInfo(
    IMAGE_LAYER_T *imageLayer,
    int32_t size,
    bool paused,
    int32_t threads,
    bool framesPerSecondValid,
    double framesPerSecond)
{
    static RGBA8_T backgroundColour = { 255, 255, 255, 255 };
    static RGBA8_T textColour = { 0, 0, 0, 255 };

    IMAGE_T *image = &(imageLayer->image);

    clearImageRGB(image, &backgroundColour);

    //---------------------------------------------------------------------

    int32_t x = INFO_LEFT_PADDING;
    int32_t y = 0;

    KEY_DIMENSIONS_T key_dimensions = { 0, 0 };

    //---------------------------------------------------------------------

    x = INFO_LEFT_PADDING;
    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "Esc", "exit");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer,
                             x,
                             y,
                             "P",
                             paused ? "resume" : "pause");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "space", "step");

    //---------------------------------------------------------------------

    char buffer[128];

    y += key_dimensions.height + INFO_TOP_PADDING;

    snprintf(buffer, sizeof(buffer), "size: %d", size);
    drawStringRGB(x, y, buffer, &textColour, image);

    y += FONT_HEIGHT + INFO_TOP_PADDING;

    snprintf(buffer, sizeof(buffer), "threads: %d", threads);
    drawStringRGB(x, y, buffer, &textColour, image);

    y += FONT_HEIGHT + INFO_TOP_PADDING;

    if (framesPerSecondValid)
    {
        snprintf(buffer, sizeof(buffer), "fps: %.f", framesPerSecond);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "fps: --");
    }

    drawStringRGB(x, y, buffer, &textColour, image);

    //---------------------------------------------------------------------

    changeSourceAndUpdateImageLayer(imageLayer);
}

