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

#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <pthread.h>
#include <stdint.h>

#include "imageLayer.h"

//-------------------------------------------------------------------------

#define MANDELBROT_MAX_THREADS 4

//-------------------------------------------------------------------------

typedef struct
{
    double x0;
    double y0;
    double side;
} MANDELBROT_COORDS_T;

//-------------------------------------------------------------------------

typedef struct
{
    int32_t startHeight;
    int32_t endHeight;
} MANDELBROT_HEIGHT_RANGE_T;

//-------------------------------------------------------------------------

typedef struct
{
    MANDELBROT_COORDS_T coords;
    IMAGE_LAYER_T *imageLayer;

    RGBA8_T colours[256];
    size_t numberOfColours;

    int32_t numberOfThreads;
    pthread_t threads[MANDELBROT_MAX_THREADS];
    MANDELBROT_HEIGHT_RANGE_T heightRange[MANDELBROT_MAX_THREADS];
    pthread_barrier_t startBarrier;
    pthread_barrier_t finishedBarrier;
} MANDELBROT_T;

//-------------------------------------------------------------------------

void
newMandelbrot(
    MANDELBROT_T *mbrot,
    IMAGE_LAYER_T *imageLayer);
void
destroyMandelbrot(
    MANDELBROT_T *mbrot);

void *
workerMandelbrot(
    void *arg);

void
mandelbrotImageKernel(
    MANDELBROT_T *mbrot,
    int32_t startHeight,
    int32_t endHeight);

void
mandelbrotImage(
    MANDELBROT_T *mbrot,
    MANDELBROT_COORDS_T *coords);

//-------------------------------------------------------------------------

#endif
