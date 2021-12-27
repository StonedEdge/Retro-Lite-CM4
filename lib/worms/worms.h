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

#ifndef WORMS_H
#define WORMS_H

//-------------------------------------------------------------------------

#include "image.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

typedef struct
{
    double x;
    double y;
} COORD_T;

typedef struct
{
    RGBA8_T colour;
    double direction;
    uint16_t head;
    uint16_t size;
    COORD_T *body;
} WORM_T;

typedef struct
{
    uint16_t size;
    WORM_T *worms;
    IMAGE_T image;
    DISPMANX_RESOURCE_HANDLE_T frontResource;
    DISPMANX_RESOURCE_HANDLE_T backResource;
    DISPMANX_ELEMENT_HANDLE_T element;
} WORMS_T;

//-------------------------------------------------------------------------

void
initWorms(
    uint16_t number,
    uint16_t length,
    WORMS_T *worms,
    VC_IMAGE_TYPE_T imageType,
    DISPMANX_MODEINFO_T *info);

void updateWorms(WORMS_T *worms);
void drawWorms(WORMS_T *worms);
void undrawWorms(WORMS_T *worms);

void
addElementWorms(
    WORMS_T *worms,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update);
void writeDataWorms(WORMS_T *worms);
void changeSourceWorms(WORMS_T *worms, DISPMANX_UPDATE_HANDLE_T update);

void destroyWorms(WORMS_T *worms);

//-------------------------------------------------------------------------

#endif
