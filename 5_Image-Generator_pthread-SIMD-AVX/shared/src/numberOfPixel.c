/*
 * FILE = /src/numberOfPixel.c
 *
 * This file holds the values for the image size and size of the shared memory
 * segment.
 * This file is used by the imageWriter and pixelGenerator program.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include "numberOfPixel.h"
#include <stddef.h>

/*
 * HEIGHT, WIDTH and size of the shared memory
 * segment can be changed (MAX_DATA must be 3 * HEIGHT * WIDTH).
 * (But keep an aspect ratio of 4/3 for HEIGHT to WIDTH)
 */

#if LARGE_IMAGE

  const int WIDTH = 2560;
  const int HEIGHT = 1920;
  const size_t MAX_DATA = 14745600; // = 3 * 2560 * 1920 = 14745600

#else

  const int WIDTH = 800;
  const int HEIGHT = 600;
  const size_t MAX_DATA = 1440000; //= 3 * 800 * 600 = 1440000

#endif
