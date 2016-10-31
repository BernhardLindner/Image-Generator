/*
 * FILE = HEADER: /include/numberOfPixel.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#ifndef _numberOfPixel_
#define _numberOfPixel_

#include <stddef.h>

/*
 * LARGE_IMAGE 0 sets image width and height to 800x600
 * LARGE_IMAGE 1 sets image width and height to 2560x1920
 */

#define LARGE_IMAGE 0

extern const int WIDTH;
extern const int HEIGHT;
extern const size_t MAX_DATA;

#endif
