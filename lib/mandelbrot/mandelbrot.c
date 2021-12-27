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

#include <assert.h>

#include "bcm_host.h"

#include "hsv2rgb.h"
#include "image.h"
#include "mandelbrot.h"

//-------------------------------------------------------------------------

void
newMandelbrot(
    MANDELBROT_T *mbrot,
    IMAGE_LAYER_T *imageLayer)
{
    mbrot->imageLayer = imageLayer;

    //---------------------------------------------------------------------

    size_t colours = (sizeof(mbrot->colours) / sizeof(mbrot->colours[0]));
    size_t colour = 0;

    mbrot->numberOfColours = colours;

    for (colour = 0 ; colour < colours ; colour++)
    {
        hsv2rgb((colours - 1 - colour) * (2400 / colours),
                1000,
                1000,
                &(mbrot->colours[colour]));
    }

    //---------------------------------------------------------------------
    
    long cores = sysconf(_SC_NPROCESSORS_ONLN);

    if (cores == -1)
    {
        cores = 1;
    }

    if (cores > MANDELBROT_MAX_THREADS)
    {
        mbrot->numberOfThreads = MANDELBROT_MAX_THREADS;
    }
    else
    {
        mbrot->numberOfThreads = cores;
    }

    pthread_barrier_init(&(mbrot->startBarrier),
                         NULL,
                         mbrot->numberOfThreads + 1);

    pthread_barrier_init(&(mbrot->finishedBarrier),
                         NULL,
                         mbrot->numberOfThreads + 1);

    //---------------------------------------------------------------------

    IMAGE_T *image = &(imageLayer->image);

    //---------------------------------------------------------------------

    int32_t heightStep = image->height / mbrot->numberOfThreads;
    int32_t heightStart = 0;

    int32_t thread;
    for (thread = 0 ; thread < mbrot->numberOfThreads ; thread++)
    {
        mbrot->heightRange[thread].startHeight = heightStart;
        mbrot->heightRange[thread].endHeight = heightStart + heightStep;

        heightStart += heightStep;

        pthread_create(&(mbrot->threads[thread]),
                       NULL,
                       workerMandelbrot,
                       mbrot);
    }

    thread = mbrot->numberOfThreads - 1;
    mbrot->heightRange[thread].endHeight = image->height;
}

//-------------------------------------------------------------------------

void
destroyMandelbrot(
    MANDELBROT_T *mbrot)
{
    int32_t thread;
    for (thread = 0 ; thread < mbrot->numberOfThreads ; thread++)
    {
        pthread_cancel(mbrot->threads[thread]);
    }
}

//-------------------------------------------------------------------------

void *
workerMandelbrot(
    void *arg)
{
    MANDELBROT_T *mbrot = arg;

    int32_t thread = -1;
    int32_t i;
    for (i = 0 ; i < mbrot->numberOfThreads ; i++)
    {
        if (pthread_equal(pthread_self(), mbrot->threads[i]))
        {
            thread = i;
            break;
        }
    }

    if (thread == -1)
    {
        fprintf(stderr, "mandelbrot: cannot find thread index\n");
        return NULL;
    }

    while (true)
    {
        pthread_barrier_wait(&(mbrot->startBarrier));
        mandelbrotImageKernel(mbrot,
                              mbrot->heightRange[thread].startHeight,
                              mbrot->heightRange[thread].endHeight);
        pthread_barrier_wait(&(mbrot->finishedBarrier));
    }

    return NULL;
}

//-------------------------------------------------------------------------

void
mandelbrotImageKernel(
    MANDELBROT_T *mbrot,
    int32_t startHeight,
    int32_t endHeight)
{
    IMAGE_T *image = &(mbrot->imageLayer->image);

    double dx = (mbrot->coords.side / (image->width - 1));
    double dy = (mbrot->coords.side / (image->height - 1));

    int32_t j;
    for (j = startHeight ; j < endHeight ; j++)
    {
        int32_t i;
        for (i = 0 ; i < image->width ; i++)
        {
            double x0 = mbrot->coords.x0 + dx * i;
            double y0 = mbrot->coords.y0 + dy * j;

            double x = 0.0;
            double y = 0.0;

            double x2 = x * x;
            double y2 = y * y;

            size_t n = 0;

            while ((x2 + y2 < 4.0) && (n < mbrot->numberOfColours))
            {
                double xtemp = x2 - y2 + x0;
                y = 2 * x * y + y0;
                x = xtemp;

                x2 = x * x;
                y2 = y * y;

                n++;
            }

            if (n < mbrot->numberOfColours)
            {
                setPixelRGB(image, i, j, &(mbrot->colours[n]));
            }
        }
    }
}

//-------------------------------------------------------------------------

void
mandelbrotImage(
    MANDELBROT_T *mbrot,
    MANDELBROT_COORDS_T *coords)
{
    memcpy(&(mbrot->coords), coords, sizeof(MANDELBROT_COORDS_T));

    static RGBA8_T black = {0, 0, 0, 0};

    IMAGE_T *image = &(mbrot->imageLayer->image);
    clearImageRGB(image, &black);

    //---------------------------------------------------------------------

    pthread_barrier_wait(&(mbrot->startBarrier));
    pthread_barrier_wait(&(mbrot->finishedBarrier));

    //---------------------------------------------------------------------

    changeSourceAndUpdateImageLayer(mbrot->imageLayer);
}

