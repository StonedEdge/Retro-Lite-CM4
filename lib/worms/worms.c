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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hsv2rgb.h"
#include "image.h"
#include "worms.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

void
initWorm(
    uint16_t worm_number,
    uint16_t number_of_worms,
    uint16_t worm_length,
    WORM_T *worm,
    IMAGE_T *image)
{
    hsv2rgb((3600 * worm_number) / number_of_worms,
            1000,
            1000,
            &(worm->colour));
    
    worm->colour.alpha = 255;
    worm->direction = (rand() * 360.0) / RAND_MAX;
    worm->head = worm_length - 1;
    worm->size = worm_length;
    worm->body = malloc(worm->size * sizeof(COORD_T));

    if (worm->body == NULL)
    {
        fprintf(stderr, "worms: memory exhausted\n");
        exit(EXIT_FAILURE);
    }

    double x = (double)rand() * image->width / RAND_MAX;
    double y = (double)rand() * image->height / RAND_MAX;

    uint16_t i = 0;
    for (i = 0 ; i < worm->size ; i++)
    {
        worm->body[i].x = x;
        worm->body[i].y = y;
    }
}

//-------------------------------------------------------------------------

void
updateWorm(
    WORM_T *worm,
    IMAGE_T *image)
{
    uint16_t wasHead = worm->head;
    worm->head = (worm->head + 1) % worm->size;

    double x = worm->body[wasHead].x;
    double y = worm->body[wasHead].y;

    //---------------------------------------------------------------------
    // randomly change direction

    worm->direction += (40.0 * rand() / RAND_MAX) - 20.0;

    if (worm->direction > 360.0)
    {
        worm->direction -= 360.0;
    }
    else if (worm->direction < 0.0)
    {
        worm->direction += 360.0;
    }

    //---------------------------------------------------------------------
    // calculate new head position

    x = x + cos(worm->direction * M_PI / 180.0);

    if (x < 0.0)
    {
        x += image->width;
    }
    else if (x > image->width)
    {
        x -= image->width;
    }

    y = y + sin(worm->direction * M_PI / 180.0);

    if (y < 0.0)
    {
        y += image->height;
    }
    else if (y > image->height)
    {
        y -= image->height;
    }

    //---------------------------------------------------------------------

    worm->body[worm->head].x = x;
    worm->body[worm->head].y = y;
}

//-------------------------------------------------------------------------

void
drawWorm(
    WORM_T *worm,
    IMAGE_T *image)
{
    uint16_t i = 0;
    for (i = 0 ; i < worm->size ; i++)
    {
        setPixelRGB(image,
                    (int32_t)floor(worm->body[i].x),
                    (int32_t)floor(worm->body[i].y),
                    &(worm->colour));
    }
}

//-------------------------------------------------------------------------

void
undrawWorm(
    WORM_T *worm,
    IMAGE_T *image)
{
    RGBA8_T colour = { 0, 0, 0, 0 };

    uint32_t i = 0;
    for (i = 0 ; i < worm->size ; i++)
    {
        setPixelRGB(image,
                    (int32_t)floor(worm->body[i].x),
                    (int32_t)floor(worm->body[i].y),
                    &colour);
    }
}

//-------------------------------------------------------------------------

void
destroyWorm(
    WORM_T* worm)
{
    free(worm->body);

    worm->head = 0;
    worm->size = 0;
    worm->body = NULL;
}

//-------------------------------------------------------------------------

void
initWorms(
    uint16_t number,
    uint16_t length,
    WORMS_T *worms,
    VC_IMAGE_TYPE_T imageType,
    DISPMANX_MODEINFO_T *info)
{
    initImage(&(worms->image), imageType, info->width, info->height, false);
    srand(time(NULL));

    worms->size = number;
    worms->worms = malloc(worms->size * sizeof(WORM_T));

    if (worms->worms == NULL)
    {
        fprintf(stderr, "worms: memory exhausted\n");
        exit(EXIT_FAILURE);
    }

    uint16_t i = 0;
    for (i = 0 ; i < worms->size ; i++)
    {
        WORM_T *worm = &(worms->worms[i]);
        initWorm(i, number, length, worm, &(worms->image));
    }

    //---------------------------------------------------------------------

    uint32_t vc_image_ptr;

    worms->frontResource =
        vc_dispmanx_resource_create(
            worms->image.type,
            worms->image.width  |(worms->image.pitch << 16),
            worms->image.height | (worms->image.alignedHeight << 16),
            &vc_image_ptr);
    assert(worms->frontResource != 0);

    worms->backResource =
        vc_dispmanx_resource_create(worms->image.type,
            worms->image.width | (worms->image.pitch << 16),
            worms->image.height | (worms->image.alignedHeight << 16),
            &vc_image_ptr);
    assert(worms->backResource != 0);

    //---------------------------------------------------------------------

    int result = 0;

    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect,
                         0,
                         0,
                         worms->image.width,
                         worms->image.height);

    result = vc_dispmanx_resource_write_data(worms->frontResource,
                                             worms->image.type,
                                             worms->image.pitch,
                                             worms->image.buffer,
                                             &dst_rect);
    assert(result == 0);
}

//-------------------------------------------------------------------------

void
addElementWorms(
    WORMS_T *worms,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 0, 0 };

    VC_RECT_T src_rect;
    vc_dispmanx_rect_set(&src_rect,
                         0,
                         0,
                         worms->image.width << 16,
                         worms->image.height << 16);

    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect,
                         0,
                         0,
                         worms->image.width,
                         worms->image.height);

    worms->element =
        vc_dispmanx_element_add(update,
                                display,
                                2000,
                                &dst_rect,
                                worms->frontResource,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL,
                                DISPMANX_NO_ROTATE);
    assert(worms->element != 0);
}

//-------------------------------------------------------------------------

void
writeDataWorms(
    WORMS_T *worms)
{
    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect,
                         0,
                         0,
                         worms->image.width,
                         worms->image.height);

    int result = vc_dispmanx_resource_write_data(worms->backResource,
                                                 worms->image.type,
                                                 worms->image.pitch,
                                                 worms->image.buffer,
                                                 &dst_rect);
    assert(result == 0);
}

//-------------------------------------------------------------------------

void
changeSourceWorms(
    WORMS_T *worms,
    DISPMANX_UPDATE_HANDLE_T update)
{
    int result = 0;
    result = vc_dispmanx_element_change_source(update,
                                               worms->element,
                                               worms->backResource);
    assert(result == 0);

    DISPMANX_RESOURCE_HANDLE_T tmp = worms->frontResource;
    worms->frontResource = worms->backResource;
    worms->backResource = tmp;
}

//-------------------------------------------------------------------------

void
updateWorms(
    WORMS_T *worms)
{
    uint16_t i = 0;
    for (i = 0 ; i < worms->size ; i++)
    {
        WORM_T *worm = &(worms->worms[i]);
        updateWorm(worm, &(worms->image));
    }
}

//-------------------------------------------------------------------------

void
drawWorms(
    WORMS_T *worms)
{
    uint16_t i = 0;
    for (i = 0 ; i < worms->size ; i++)
    {
        WORM_T *worm = &(worms->worms[i]);
        drawWorm(worm, &(worms->image));
    }
}

//-------------------------------------------------------------------------

void
undrawWorms(
    WORMS_T *worms)
{
    uint16_t i = 0;
    for (i = 0 ; i < worms->size ; i++)
    {
        WORM_T *worm = &(worms->worms[i]);
        undrawWorm(worm, &(worms->image));
    }
}

//-------------------------------------------------------------------------

void
destroyWorms(
    WORMS_T *worms)
{
    uint16_t i = 0;
    for (i = 0 ; i < worms->size ; i++)
    {
        WORM_T *worm = &(worms->worms[i]);
        destroyWorm(worm);
    }

    free(worms->worms);

    worms->size = 0;
    worms->worms = NULL;

    destroyImage(&(worms->image));


    //---------------------------------------------------------------------

    int result = 0;

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
    result = vc_dispmanx_element_remove(update, worms->element);
    assert(result == 0);
    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    result = vc_dispmanx_resource_delete(worms->frontResource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(worms->backResource);
    assert(result == 0);
}

