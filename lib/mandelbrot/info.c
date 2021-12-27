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
calculatingInfo(
    IMAGE_LAYER_T *imageLayer,
    int32_t threads)
{
    static RGBA8_T textColour = { 0, 0, 0, 255 };
    static RGBA8_T backgroundColour = { 255, 255, 255, 255 };

    IMAGE_T *image = &(imageLayer->image);

    clearImageRGB(image, &backgroundColour);

    //---------------------------------------------------------------------

    drawStringRGB(INFO_LEFT_PADDING,
                  INFO_TOP_PADDING,
                  "Calculating ...",
                  &textColour,
                  image);

    //---------------------------------------------------------------------

    char buffer[128];

    snprintf(buffer, sizeof(buffer), "threads: %d", threads);

    drawStringRGB(INFO_LEFT_PADDING,
                  (2 * INFO_TOP_PADDING) + FONT_HEIGHT,
                  buffer,
                  &textColour,
                  image);

    changeSourceAndUpdateImageLayer(imageLayer);
}

//-------------------------------------------------------------------------

void
mandelbrotInfo(
    IMAGE_LAYER_T *imageLayer)
{
    static RGBA8_T backgroundColour = { 255, 255, 255, 255 };

    IMAGE_T *image = &(imageLayer->image);

    clearImageRGB(image, &backgroundColour);

    //---------------------------------------------------------------------

    KEY_DIMENSIONS_T key_dimensions = { 0, 0 };
    int32_t x = INFO_LEFT_PADDING;
    int32_t y = 0;

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "Esc", "exit");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "Z", "zoom in");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "S", "save PNG file");

    //---------------------------------------------------------------------

    changeSourceAndUpdateImageLayer(imageLayer);
}

//-------------------------------------------------------------------------

void
zoomInfo(
    IMAGE_LAYER_T *imageLayer,
    int32_t *steps,
    int32_t numberOfSteps,
    int32_t stepIndex)
{
    static RGBA8_T backgroundColour = { 255, 255, 255, 255 };

    IMAGE_T *image = &(imageLayer->image);

    clearImageRGB(image, &backgroundColour);

    //---------------------------------------------------------------------

    int32_t x = INFO_LEFT_PADDING;
    int32_t y = 0;

    KEY_DIMENSIONS_T key_dimensions = { 0, 0 };

    //---------------------------------------------------------------------

    x = INFO_LEFT_PADDING;
    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "Enter", "zoom");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "Esc", "cancel zoom");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "W", "move up");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "A", "move left");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "S", "move down");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "D", "mode right");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "[", "decrease step");

    //---------------------------------------------------------------------

    y += key_dimensions.height + INFO_TOP_PADDING;

    key_dimensions = drawKey(imageLayer, x, y, "]", "increase step");

    //---------------------------------------------------------------------

    static RGBA8_T lowlightColour = { 191, 191, 191, 255 };
    static RGBA8_T highlightColour = { 0, 0, 0, 255 };

    y += key_dimensions.height + INFO_TOP_PADDING;

    drawStringRGB(x, y, "step:", &highlightColour, image);

    y += FONT_HEIGHT + INFO_TOP_PADDING;

    char buffer[128];

    int32_t i = 0;
    for (i = 0 ; i < numberOfSteps ; i++)
    {
        RGBA8_T *colour = &lowlightColour;

        if (i == stepIndex)
        {
            colour = &highlightColour;
        }

        snprintf(buffer, sizeof(buffer), "%d ", steps[i]);

        drawStringRGB(x, y, buffer, colour, image);

        x += strlen(buffer) * FONT_WIDTH;
    }

    //---------------------------------------------------------------------

    changeSourceAndUpdateImageLayer(imageLayer);
}

