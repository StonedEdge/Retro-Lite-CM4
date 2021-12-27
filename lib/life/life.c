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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "life.h"

//-------------------------------------------------------------------------

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

//-------------------------------------------------------------------------

#define LIVE 210
#define DEAD 3

//-------------------------------------------------------------------------

static void
setCell(
    LIFE_T *life,
    int32_t col,
    int32_t row)
{
    life->buffer[col + (row * life->alignedWidth)] = LIVE;

    int32_t width = life->width;
    int32_t height = life->height;
    int32_t fieldLength = life->fieldLength;

    uint8_t *cell = life->fieldNext + col + (row * width);

    uint32_t left = (col == 0) ? width - 1 :  -1;
    uint32_t right = (col == width - 1) ?  -(width - 1) : 1;
    uint32_t above = (row == 0) ? fieldLength - width : -width;
    uint32_t below = (row == height - 1) ? -(fieldLength - width) : width;

    *(cell) |= 0x01;
    *(cell + above + left) += 2;
    *(cell + above) += 2;
    *(cell + above + right) += 2;
    *(cell + left) += 2;
    *(cell + right) += 2;
    *(cell + below + left) += 2;
    *(cell + below) += 2;
    *(cell + below + right) += 2;
}

//-------------------------------------------------------------------------

static void
clearCell(
    LIFE_T *life,
    int32_t col,
    int32_t row)
{
    life->buffer[col + (row * life->alignedWidth)] = DEAD;

    int32_t width = life->width;
    int32_t height = life->height;
    int32_t fieldLength = life->fieldLength;

    uint8_t *cell = life->fieldNext + col + (row * width);

    uint32_t left = (col == 0) ? width - 1 :  -1;
    uint32_t right = (col == width - 1) ?  -(width - 1) : 1;
    uint32_t above = (row == 0) ? fieldLength - width : -width;
    uint32_t below = (row == height - 1) ? -(fieldLength - width) : width;

    *(cell) &= ~0x01;
    *(cell + above + left) -= 2;
    *(cell + above) -= 2;
    *(cell + above + right) -= 2;
    *(cell + left) -= 2;
    *(cell + right) -= 2;
    *(cell + below + left) -= 2;
    *(cell + below) -= 2;
    *(cell + below + right) -= 2;
}

//-------------------------------------------------------------------------

void
newLife(
    LIFE_T *life,
    int32_t size)
{
    life->width = size;
    life->height = size;
    life->alignedWidth = ALIGN_TO_16(life->width);
    life->alignedHeight = ALIGN_TO_16(life->height);
    life->pitch = ALIGN_TO_16(life->width);

    life->buffer = calloc(1, life->pitch * life->alignedHeight);

    if (life->buffer == NULL)
    {
        fprintf(stderr, "life: memory exhausted\n");
        exit(EXIT_FAILURE);
    }

    life->fieldLength = life->width * life->height;

    life->field = calloc(1, life->fieldLength);

    if (life->field == NULL)
    {
        fprintf(stderr, "life: memory exhausted\n");
        exit(EXIT_FAILURE);
    }

    life->fieldNext = calloc(1, life->fieldLength);

    if (life->fieldNext == NULL)
    {
        fprintf(stderr, "life: memory exhausted\n");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    int32_t row = 0;
    for (row = 0 ; row < life->height ; row++)
    {
        int32_t col = 0;
        for (col = 0 ; col < life->width ; col++)
        {
            if (rand() > (RAND_MAX / 2))
            {
                setCell(life, col, row);
            }
            else
            {
                life->buffer[col + (row * life->alignedWidth)] = DEAD;
            }
        }
    }

    //---------------------------------------------------------------------

    VC_IMAGE_TYPE_T type = VC_IMAGE_8BPP;
    uint32_t vc_image_ptr;
    int result = 0;

    life->frontResource = 
        vc_dispmanx_resource_create(
            type,
            life->width | (life->pitch << 16),
            life->height | (life->alignedHeight << 16),
            &vc_image_ptr);
    assert(life->frontResource != 0);

    life->backResource = 
        vc_dispmanx_resource_create(
            type,
            life->width | (life->pitch << 16),
            life->height | (life->alignedHeight << 16),
            &vc_image_ptr);
    assert(life->backResource != 0);

    //---------------------------------------------------------------------

    vc_dispmanx_rect_set(&(life->bmpRect), 0, 0, life->width, life->height);

    result = vc_dispmanx_resource_write_data(life->frontResource,
                                             type,
                                             life->pitch,
                                             life->buffer,
                                             &(life->bmpRect));
    assert(result == 0);

    //---------------------------------------------------------------------

    long cores = sysconf(_SC_NPROCESSORS_ONLN);

    if (cores == -1)
    {
        cores = 1;
    }

    if (cores > LIFE_MAX_THREADS)
    {
        life->numberOfThreads = LIFE_MAX_THREADS;
    }
    else
    {
        life->numberOfThreads = cores;
    }

    pthread_barrier_init(&(life->startIterationBarrier),
                         NULL,
                         life->numberOfThreads + 1);

    pthread_barrier_init(&(life->finishedIterationBarrier),
                         NULL,
                         life->numberOfThreads + 1);

    //---------------------------------------------------------------------

    int32_t heightStep = life->height / life->numberOfThreads;
    int32_t heightStart = 0;

    int32_t thread;
    for (thread = 0 ; thread < life->numberOfThreads ; thread++)
    {
        life->heightRange[thread].startHeight = heightStart;
        life->heightRange[thread].endHeight = heightStart + heightStep;

        heightStart += heightStep;

        pthread_create(&(life->threads[thread]),
                       NULL,
                       workerLife,
                       life);
    }

    thread = life->numberOfThreads - 1;
    life->heightRange[thread].endHeight = life->height;

    //---------------------------------------------------------------------

    memcpy(life->field, life->fieldNext, life->fieldLength);
    pthread_barrier_wait(&(life->startIterationBarrier));
}

//-------------------------------------------------------------------------

void *
workerLife(
    void *arg)
{
    LIFE_T *life = arg;

    int32_t thread = -1;
    int32_t i;
    for (i = 0 ; i < life->numberOfThreads ; i++)
    {
        if (pthread_equal(pthread_self(), life->threads[i]))
        {
            thread = i;
            break;
        }
    }

    if (thread == -1)
    {
        fprintf(stderr, "life: cannot find thread index\n");
        return NULL;
    }

    while (true)
    {
        pthread_barrier_wait(&(life->startIterationBarrier));
        iterateLifeKernel(life, thread);
        pthread_barrier_wait(&(life->finishedIterationBarrier));
    }

    return NULL;
}

//-------------------------------------------------------------------------

void
addElementLife(
    LIFE_T *life,
    int32_t xOffset,
    int32_t yOffset,
    int32_t dstSize,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    VC_DISPMANX_ALPHA_T alpha =
    {
        DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 
        255, /*alpha 0->255*/
        0
    };

    //---------------------------------------------------------------------

    vc_dispmanx_rect_set(&(life->srcRect),
                         0,
                         0,
                         life->width << 16,
                         life->height << 16);

    vc_dispmanx_rect_set(&(life->dstRect),
                         xOffset,
                         yOffset,
                         dstSize,
                         dstSize);

    life->element =
        vc_dispmanx_element_add(update,
                                display,
                                1,
                                &(life->dstRect),
                                life->frontResource,
                                &(life->srcRect),
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL,
                                DISPMANX_NO_ROTATE);
    assert(life->element != 0);
}

//-------------------------------------------------------------------------

void
iterateLifeKernel(
    LIFE_T *life,
    int32_t thread)
{
    uint8_t *cell = life->field +
                    (life->heightRange[thread].startHeight * life->width);

    int32_t row;
    for (row = life->heightRange[thread].startHeight ;
         row < life->heightRange[thread].endHeight ;
         row++)
    {
        int32_t col;
        for (col = 0 ; col < life->width ; col++)
        {
            uint8_t neighbours = *cell >> 1;

            if (*cell & 0x01)
            {
                if ((neighbours != 2) && (neighbours != 3))
                {
                    clearCell(life, col, row);
                }
            }
            else
            {
                if (neighbours == 3)
                {
                    setCell(life, col, row);
                }
            }

            ++cell;
        }
    }
}

//-------------------------------------------------------------------------

void
iterateLife(
    LIFE_T *life)
{
    pthread_barrier_wait(&(life->finishedIterationBarrier));

    int result = 0;
    VC_IMAGE_TYPE_T type = VC_IMAGE_RGBA16;

    result = vc_dispmanx_resource_write_data(life->backResource,
                                             type,
                                             life->pitch,
                                             life->buffer,
                                             &(life->bmpRect));
    assert(result == 0);

    memcpy(life->field, life->fieldNext, life->fieldLength);
    pthread_barrier_wait(&(life->startIterationBarrier));
}

//-------------------------------------------------------------------------

void
changeSourceLife(
    LIFE_T *life,
    DISPMANX_UPDATE_HANDLE_T update)
{
    int result = 0;
    result = vc_dispmanx_element_change_source(update,
                                               life->element,
                                               life->backResource);
    assert(result == 0);

    // swap front and back resources

    DISPMANX_RESOURCE_HANDLE_T tmp = life->frontResource;
    life->frontResource = life->backResource;
    life->backResource = tmp;
}

//-------------------------------------------------------------------------

void
destroyLife(
    LIFE_T *life)
{
    if (life->buffer)
    {
        free(life->buffer);
        life->buffer = NULL;
    }

    if (life->field)
    {
        free(life->field);
        life->field = NULL;
    }

    if (life->fieldNext)
    {
        free(life->fieldNext);
        life->fieldNext = NULL;
    }

    life->width = 0;
    life->alignedWidth = 0;
    life->height = 0;
    life->alignedHeight = 0;
    life->pitch = 0;
    life->fieldLength = 0;

    //---------------------------------------------------------------------

    int result = 0;

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
    result = vc_dispmanx_element_remove(update, life->element);
    assert(result == 0);
    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    result = vc_dispmanx_resource_delete(life->frontResource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(life->backResource);
    assert(result == 0);

    //---------------------------------------------------------------------

    int32_t thread;
    for (thread = 0 ; thread < life->numberOfThreads ; thread++)
    {
        pthread_cancel(life->threads[thread]);
    }
}

