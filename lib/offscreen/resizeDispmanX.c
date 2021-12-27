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

#define _GNU_SOURCE

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bcm_host.h"

#include "resizeDispmanX.h"

//-------------------------------------------------------------------------

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

//-------------------------------------------------------------------------

void
initResizeDispmanX(
    RESIZE_DISPMANX_T *rd,
    VC_IMAGE_TYPE_T type,
    int16_t dWidth,
    int16_t dHeight,
    int16_t sWidth,
    int16_t sHeight,
    bool keepAspectRatio)
{
    int16_t width = dWidth;
    int16_t height = dHeight;

    rd->xRatio = (((int32_t)sWidth << 16) / width) + 1;
    rd->yRatio = (((int32_t)sHeight << 16) / height) + 1;

    rd->destinationWidth = ALIGN_TO_16(width);
    rd->destinationHeight = height;

    if (keepAspectRatio)
    {
        if (rd->xRatio < rd->yRatio)
        {
            rd->xRatio = rd->yRatio;
            rd->destinationWidth = ALIGN_TO_16((sWidth * rd->destinationHeight) / sHeight);
        }
        else
        {
            rd->yRatio = rd->xRatio;
            rd->destinationHeight = (rd->destinationWidth * sHeight) / sWidth;
        }

    }

    rd->sourceWidth = sWidth;
    rd->sourceHeight = sHeight;

    bcm_host_init();

    rd->type = type;

    uint32_t dstImageHandle;

    rd->dstRes = vc_dispmanx_resource_create(rd->type,
                                             rd->destinationWidth,
                                             rd->destinationHeight,
                                             &dstImageHandle);

    uint32_t srcImageHandle;

    rd->srcRes = vc_dispmanx_resource_create(rd->type,
                                             sWidth,
                                             sHeight,
                                             &srcImageHandle);

    rd->display = vc_dispmanx_display_open_offscreen(rd->dstRes,
                                                     DISPMANX_NO_ROTATE);

    vc_dispmanx_rect_set(&(rd->bmpRect), 0, 0, sWidth, sHeight);
    vc_dispmanx_rect_set(&(rd->srcRect), 0, 0, sWidth << 16, sHeight << 16);

    vc_dispmanx_rect_set(&(rd->dstRect),
                         0,
                         0,
                         rd->destinationWidth,
                         rd->destinationHeight);
}

//-------------------------------------------------------------------------

void
destroyResizeDispmanX(
    RESIZE_DISPMANX_T *rd)
{
    vc_dispmanx_display_close(rd->display);
    vc_dispmanx_resource_delete(rd->srcRes);
    vc_dispmanx_resource_delete(rd->dstRes);
}

//-------------------------------------------------------------------------

void
resizeDispmanX(
    RESIZE_DISPMANX_T *rd,
    IMAGE_T *dst,
    IMAGE_T *src)
{
    vc_dispmanx_resource_write_data(rd->srcRes,
                                    rd->type,
                                    src->pitch,
                                    src->buffer,
                                    &(rd->bmpRect));

    VC_DISPMANX_ALPHA_T alpha;

    alpha.mask = DISPMANX_NO_HANDLE;
    alpha.flags = DISPMANX_FLAGS_ALPHA_FROM_SOURCE
                | DISPMANX_FLAGS_ALPHA_MIX;
    alpha.opacity = 255;

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

    DISPMANX_ELEMENT_HANDLE_T element;
    element = vc_dispmanx_element_add(update,
                                      rd->display,
                                      0,
                                      &(rd->dstRect),
                                      rd->srcRes,
                                      &(rd->srcRect),
                                      DISPMANX_PROTECTION_NONE,
                                      &alpha,
                                      NULL,
                                      DISPMANX_NO_ROTATE);

    vc_dispmanx_update_submit_sync(update);

    vc_dispmanx_resource_read_data(rd->dstRes,
                                   &(rd->dstRect),
                                   dst->buffer,
                                   dst->pitch);

    vc_dispmanx_element_remove(update, element);
}

//-------------------------------------------------------------------------

