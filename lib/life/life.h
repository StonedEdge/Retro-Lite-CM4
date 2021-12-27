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

#ifndef LIFE_H
#define LIFE_H

#include <pthread.h>
#include <stdint.h>

#include "bcm_host.h"

//-------------------------------------------------------------------------

#define LIFE_MAX_THREADS 4

//-------------------------------------------------------------------------

typedef struct
{
    int32_t startHeight;
    int32_t endHeight;
} LIFE_HEIGHT_RANGE_T;

//-------------------------------------------------------------------------

typedef struct
{
    int32_t width;
    int32_t alignedWidth;
    int32_t height;
    int32_t alignedHeight;
    int32_t pitch;
    uint8_t *buffer;
    int32_t fieldLength;
    uint8_t *field;
    uint8_t *fieldNext;

    VC_RECT_T bmpRect;
    VC_RECT_T srcRect;
    VC_RECT_T dstRect;
    DISPMANX_RESOURCE_HANDLE_T frontResource;
    DISPMANX_RESOURCE_HANDLE_T backResource;
    DISPMANX_ELEMENT_HANDLE_T element;

    int32_t numberOfThreads;
    pthread_t threads[LIFE_MAX_THREADS];
    LIFE_HEIGHT_RANGE_T heightRange[LIFE_MAX_THREADS];
    pthread_barrier_t startIterationBarrier;
    pthread_barrier_t finishedIterationBarrier;
} LIFE_T;

//-------------------------------------------------------------------------

void newLife(LIFE_T *life, int32_t size);

void
addElementLife(
    LIFE_T *life,
    int32_t xOffset,
    int32_t yOffset,
    int32_t dstSize,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update);

void *
workerLife(
    void *arg);

void
iterateLifeKernel(
    LIFE_T *life,
    int32_t thread);

void
iterateLife(
    LIFE_T *life);

void
changeSourceLife(
    LIFE_T *life,
    DISPMANX_UPDATE_HANDLE_T update);

void destroyLife(LIFE_T *life);

//-------------------------------------------------------------------------

#endif
