//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2014 Andrew Duncan
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

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "bcm_host.h"

#include "image.h"
#include "imagePalette.h"
#include "key.h"

//-----------------------------------------------------------------------

#define NDEBUG

//-----------------------------------------------------------------------

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

//-----------------------------------------------------------------------

const char* program = NULL;

//-----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    uint32_t displayNumber = 0;

    program = basename(argv[0]);

    //-------------------------------------------------------------------

    int opt;

    while ((opt = getopt(argc, argv, "d:")) != -1)
    {
        switch (opt)
        {
        case 'd':

            displayNumber = atoi(optarg);
            break;

        default:

            fprintf(stderr, "Usage: %s [-d <number>]\n", program);
            fprintf(stderr, "    -d - Raspberry Pi display number\n");
            exit(EXIT_FAILURE);
            break;
        }
    }

    //-------------------------------------------------------------------

    VC_IMAGE_TYPE_T imageType = VC_IMAGE_8BPP;

    int result = 0;

    //-------------------------------------------------------------------

    bcm_host_init();

    DISPMANX_DISPLAY_HANDLE_T displayHandle
        = vc_dispmanx_display_open(displayNumber);
    assert(displayHandle != 0);

    DISPMANX_MODEINFO_T modeInfo;
    result = vc_dispmanx_display_get_info(displayHandle, &modeInfo);
    assert(result == 0);

    //-------------------------------------------------------------------

    int32_t size = modeInfo.height;

    if (modeInfo.width < modeInfo.height)
    {
        size = modeInfo.width;
    }

    IMAGE_T image;
    initImage(&image, imageType, size, size, false);

    int32_t xOffset = (modeInfo.width - image.width) / 2;
    int32_t yOffset = (modeInfo.height - image.height) / 2;

    //---------------------------------------------------------------------

    uint32_t vc_image_ptr;
    uint32_t background = 0;

    DISPMANX_RESOURCE_HANDLE_T bgResource =
        vc_dispmanx_resource_create(VC_IMAGE_RGB565,
                                    1,
                                    1,
                                    &vc_image_ptr);
    assert(bgResource != 0);

    //---------------------------------------------------------------------

    uint32_t vcImagePtr = 0;

    DISPMANX_RESOURCE_HANDLE_T resource =
        vc_dispmanx_resource_create(
            imageType,
            image.width | (image.pitch << 16),
            image.height | (image.alignedHeight << 16),
            &vcImagePtr);
    assert(resource != 0);

    //---------------------------------------------------------------------

    int32_t r = size / 2;
    int32_t r2 = r * r;

    int32_t y;
    for (y = 0 ; y < image.height ; y++)
    {
        int32_t x;
        for (x = 0 ; x < image.width ; x++)
        {
            int32_t i = x - (size / 2);
            int32_t j = y - (size / 2);

            if (((i * i) + (j * j)) <= r2)
            {
                double angle = atan2(j, i) + M_PI;
                int32_t i = (int32_t)(floor(angle * 254.0 / (2*M_PI))) + 1;

                if (i > 255)
                {
                    i = 255;
                }

                setPixelIndexed(&image, x, y, i);
            }
        }
    }

    //---------------------------------------------------------------------

    IMAGE_PALETTE32_T palette;
    initImagePalette32(&palette, 512);

    int p;
    for (p = 1 ; p < 64 ; p++)
    {
        RGBA8_T rgba;

        rgba.red = 0;
        rgba.green = 255;
        rgba.blue = 0;
        rgba.alpha = (p * 255) / 63;

        setPalette32EntryRgba(&palette, p, &rgba);
        setPalette32EntryRgba(&palette, p + 254, &rgba);
    }

    //---------------------------------------------------------------------

    VC_RECT_T bmp_rect;

    vc_dispmanx_rect_set(&bmp_rect, 0, 0, 1, 1);

    result = vc_dispmanx_resource_write_data(bgResource,
                                             imageType,
                                             sizeof(background),
                                             &background,
                                             &bmp_rect);
    assert(result == 0);

    //-------------------------------------------------------------------

    vc_dispmanx_rect_set(&bmp_rect, 0, 0, image.width, image.height);

    result = vc_dispmanx_resource_write_data(resource,
                                             image.type,
                                             image.pitch,
                                             image.buffer,
                                             &bmp_rect);
    assert(result == 0);

    //-------------------------------------------------------------------

    setResourcePalette32(&palette, 0, resource, 1, 256);

    //-------------------------------------------------------------------

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);

    //---------------------------------------------------------------------

    VC_DISPMANX_ALPHA_T alpha;
    alpha.flags = DISPMANX_FLAGS_ALPHA_FROM_SOURCE;
    alpha.opacity = 255;
    alpha.mask = 0;

    //---------------------------------------------------------------------

    VC_RECT_T src_rect;
    VC_RECT_T dst_rect;

    vc_dispmanx_rect_set(&src_rect, 0, 0, 1, 1);
    vc_dispmanx_rect_set(&dst_rect, 0, 0, 0, 0);

    DISPMANX_ELEMENT_HANDLE_T bgElement = 
        vc_dispmanx_element_add(update,
                                displayHandle,
                                1,
                                &dst_rect,
                                bgResource,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL,
                                DISPMANX_NO_ROTATE);
    assert(bgElement != 0);

    //---------------------------------------------------------------------

    vc_dispmanx_rect_set(&src_rect,
                        0,
                        0,
                        image.width << 16,
                        image.height << 16);

    vc_dispmanx_rect_set(&dst_rect,
                         xOffset,
                         yOffset,
                         image.width,
                         image.height);

    DISPMANX_ELEMENT_HANDLE_T element =
        vc_dispmanx_element_add(update,
                                displayHandle,
                                2,
                                &dst_rect,
                                resource,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL,
                                DISPMANX_NO_ROTATE);
    assert(element != 0);

    //---------------------------------------------------------------------

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    int c = 0;
    int offset = 255;
    while (c != 27)
    {
        keyPressed(&c);

        //-----------------------------------------------------------

        setResourcePalette32(&palette, offset, resource, 1, 256);

        offset--;
        if (offset < 1)
        {
            offset = 255;
        }

        DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
        assert(update != 0);

        result = vc_dispmanx_element_change_source(update,
                                                   element,
                                                   resource);
        assert(result == 0);
        vc_dispmanx_update_submit_sync(update);
        assert(result == 0);

        //-----------------------------------------------------------
    }

    update = vc_dispmanx_update_start(0);
    assert(update != 0);
    result = vc_dispmanx_element_remove(update, bgElement);
    assert(result == 0);
    result = vc_dispmanx_element_remove(update, element);
    assert(result == 0);
    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //-------------------------------------------------------------------

    result = vc_dispmanx_resource_delete(resource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(bgResource);
    assert(result == 0);

    result = vc_dispmanx_display_close(displayHandle);
    assert(result == 0);

    //-------------------------------------------------------------------

    destroyImage(&image);
    destroyImagePalette32(&palette);

    //-------------------------------------------------------------------

    keyboardReset();

    //-------------------------------------------------------------------

    return 0;
}

