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

#ifndef RESIZE_DISPMANX_H
#define RESIZE_DISPMANX_H

//-------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

#include "bcm_host.h"

#include "image.h"

//-------------------------------------------------------------------------

typedef struct
{
    int16_t destinationWidth;
    int16_t destinationHeight;
    int16_t sourceWidth;
    int16_t sourceHeight;
    int32_t xRatio;
    int32_t yRatio;
    VC_IMAGE_TYPE_T type;
    DISPMANX_RESOURCE_HANDLE_T dstRes;
    DISPMANX_RESOURCE_HANDLE_T srcRes;
    DISPMANX_DISPLAY_HANDLE_T display;
    VC_RECT_T bmpRect;
    VC_RECT_T srcRect;
    VC_RECT_T dstRect;
} RESIZE_DISPMANX_T;

//-------------------------------------------------------------------------

void
initResizeDispmanX(
    RESIZE_DISPMANX_T *rd,
    VC_IMAGE_TYPE_T type,
    int16_t dWidth,
    int16_t dHeight,
    int16_t sWidth,
    int16_t sHeight,
    bool keepAspectRatio);

void
destroyResizeDispmanX(
    RESIZE_DISPMANX_T *rd);

void
resizeDispmanX(
    RESIZE_DISPMANX_T *rd,
    IMAGE_T *dst,
    IMAGE_T *src);

//-------------------------------------------------------------------------

#endif

