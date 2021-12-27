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

#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "bcm_host.h"

#include "element_change.h"
#include "image.h"
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
    int opt = 0;

    uint32_t displayNumber = 0;
    int32_t requestedSize = 256;
    bool animate = false;
    bool dither = false;
    const char* imageTypeName = "RGB888";
    VC_IMAGE_TYPE_T imageType = VC_IMAGE_MIN;

    int result = 0;

    program = basename(argv[0]);

    //-------------------------------------------------------------------

    while ((opt = getopt(argc, argv, "adD:s:t:")) != -1)
    {
        switch (opt)
        {
        case 'a':

            animate = true;
            break;

        case 'd':

            dither = true;
            break;

        case 'D':

            displayNumber = atoi(optarg);
            break;

        case 's':

            requestedSize = atoi(optarg);
            break;

        case 't':

            imageTypeName = optarg;
            break;

        default:

            fprintf(stderr, "Usage: %s ", program);
            fprintf(stderr, "[-a] [-d] [-D <number>] [-s <size>] ");
            fprintf(stderr, "[-t <type>]\n");
            fprintf(stderr, "    -a - animate\n");
            fprintf(stderr, "    -d - dither\n");
            fprintf(stderr, "    -D - Raspberry Pi display number\n");
            fprintf(stderr, "    -s - size of triangle to draw\n");
            fprintf(stderr, "    -t - type of image to create\n");
            fprintf(stderr, "         can be one of the following:");
            printImageTypes(stderr, " ", "", IMAGE_TYPES_ALL_DIRECT_COLOUR);
            fprintf(stderr, "\n");

            exit(EXIT_FAILURE);
            break;
        }
    }

    //-------------------------------------------------------------------

    IMAGE_TYPE_INFO_T typeInfo;

    if (findImageType(&typeInfo,
                      imageTypeName,
                      IMAGE_TYPES_ALL_DIRECT_COLOUR))
    {
        imageType = typeInfo.type;
    }
    else
    {
        fprintf(stderr,
                "%s: unknown image type %s\n",
                program,
                imageTypeName);

        exit(EXIT_FAILURE);
    }

    //-------------------------------------------------------------------

    IMAGE_T image;
    initImage(&image, imageType, 256, 256, dither);

    //-------------------------------------------------------------------

    bcm_host_init();

    DISPMANX_DISPLAY_HANDLE_T displayHandle
        = vc_dispmanx_display_open(displayNumber);
    assert(displayHandle != 0);

    DISPMANX_MODEINFO_T modeInfo;
    result = vc_dispmanx_display_get_info(displayHandle, &modeInfo);
    assert(result == 0);

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

    DISPMANX_RESOURCE_HANDLE_T frontResource =
        vc_dispmanx_resource_create(
            imageType,
            image.width | (image.pitch << 16),
            image.height | (image.alignedHeight << 16),
            &vcImagePtr);
    assert(frontResource != 0);

    DISPMANX_RESOURCE_HANDLE_T backResource =
        vc_dispmanx_resource_create(
            imageType,
            image.width | (image.pitch << 16),
            image.height | (image.alignedHeight << 16),
            &vcImagePtr);
    assert(backResource != 0);

    //-------------------------------------------------------------------

    int32_t width = requestedSize;
    int32_t height = requestedSize;

    if ((requestedSize == 0) || 
        (requestedSize > width) ||
        (requestedSize > height))
    {
        if (height > width)
        {
            width = modeInfo.width;
            height = modeInfo.width;
        }
        else
        {
            width = modeInfo.height;
            height = modeInfo.height;
        }
    }

    int32_t x_offset = (modeInfo.width - width) / 2;
    int32_t y_offset = (modeInfo.height - height) / 2;

    int32_t y;
    for (y = 0 ; y < 256 ; y++)
    {
        int32_t x;
        for (x = 0 ; x < 256 ; x++)
        {
            RGBA8_T rgba;

            if (((255 - x - (y/2)) >= 0)&& ((x - (y/2)) >= 0))
            {
                rgba.red = 255 - x - (y/2);
                rgba.green = x - (y/2);
                rgba.blue = 255 - rgba.red - rgba.green;
                rgba.alpha = 255;
            }
            else
            {
                rgba.red = 0;
                rgba.green = 0;
                rgba.blue = 0;
                rgba.alpha = 255;
            }

            setPixelRGB(&image, x, y, &rgba);
        }
    }

    //---------------------------------------------------------------------

    VC_RECT_T bmp_rect;
    VC_RECT_T src_rect;
    VC_RECT_T dst_rect;


    vc_dispmanx_rect_set(&bmp_rect, 0, 0, 1, 1);

    result = vc_dispmanx_resource_write_data(bgResource,
                                             imageType,
                                             sizeof(background),
                                             &background,
                                             &bmp_rect);
    assert(result == 0);

    //-------------------------------------------------------------------

    vc_dispmanx_rect_set(&src_rect, 0, 0, image.width, image.height);

    result = vc_dispmanx_resource_write_data(frontResource,
                                             image.type,
                                             image.pitch,
                                             image.buffer,
                                             &src_rect);
    assert(result == 0);

    result = vc_dispmanx_resource_write_data(backResource,
                                             image.type,
                                             image.pitch,
                                             image.buffer,
                                             &src_rect);
    assert(result == 0);

    //-------------------------------------------------------------------

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);

    //---------------------------------------------------------------------

    VC_DISPMANX_ALPHA_T alpha;
    alpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
    alpha.opacity = 255;
    alpha.mask = 0;

    //---------------------------------------------------------------------

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
                         x_offset,
                         y_offset,
                         width,
                         height);

    DISPMANX_ELEMENT_HANDLE_T element =
        vc_dispmanx_element_add(update,
                                displayHandle,
                                2,
                                &dst_rect,
                                frontResource,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL,
                                DISPMANX_NO_ROTATE);
    assert(element != 0);

    //---------------------------------------------------------------------

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    int32_t direction = 1;
    int c = 0;
    bool size_changed = false;

    while (c != 27)
    {
        if (keyPressed(&c))
        {
            c = tolower(c);

            switch (tolower(c))
            {
            case 'a':

                if (animate)
                {
                    animate = false;
                }
                else
                {
                    animate = true;
                }

                break;

            case '+':

                if ((width < modeInfo.width) && (height < modeInfo.height))
                {
                    width += 1;
                    height += 1;
                    y_offset = (modeInfo.height - height) / 2;
                    size_changed = true;
                }

                break;

            case '-':

                if ((width > 1) && (height > 1))
                {
                    width -= 1;
                    height -= 1;
                    y_offset = (modeInfo.height - height) / 2;
                    size_changed = true;
                }

                break;

            default:

                // do nothing

                break;
            }
        }

        //-----------------------------------------------------------

        if (animate) 
        {
            x_offset += direction;

            if (x_offset < -width )
            {
                direction = 1;
            }
            else if (x_offset > modeInfo.width)
            {
                direction = -1;
            }
        }

        if (animate || size_changed)
        {
            size_changed = false;

            vc_dispmanx_rect_set(&dst_rect,
                                 x_offset,
                                 y_offset,
                                 width,
                                 height);

            //-----------------------------------------------------------

            DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
            assert(update != 0);

            result =
            vc_dispmanx_element_change_attributes(update,
                                                  element,
                                                  ELEMENT_CHANGE_DEST_RECT,
                                                  0,
                                                  0,
                                                  &dst_rect,
                                                  NULL,
                                                  0,
                                                  DISPMANX_NO_ROTATE);
            assert(result == 0);

            result = vc_dispmanx_element_change_source(update,
                                                       element,
                                                       backResource);
            assert(result == 0);
            vc_dispmanx_update_submit_sync(update);
            assert(result == 0);

            //-----------------------------------------------------------

            DISPMANX_RESOURCE_HANDLE_T tmpResource = frontResource;
            frontResource = backResource;
            backResource = tmpResource;
        }
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

    result = vc_dispmanx_resource_delete(frontResource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(backResource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(bgResource);
    assert(result == 0);

    result = vc_dispmanx_display_close(displayHandle);
    assert(result == 0);

    //-------------------------------------------------------------------

    destroyImage(&image);

    //-------------------------------------------------------------------

    keyboardReset();

    //-------------------------------------------------------------------

    return 0;
}

