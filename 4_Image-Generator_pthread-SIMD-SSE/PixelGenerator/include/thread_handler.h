/*
 * FILE = HEADER: /include/thread_handler.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#ifndef _thread_handler_
#define _thread_handler_

#include <pthread.h>

/*
 * The computation of the mandelbrot set is done by multiple threads. I have
 * set the number of threads to 8 but changing it to 1, 2 or 8 is also possible.
 * If the HEIGHT of the Image (which is set to 600 -> numberOfPixel.c)
 * divided by the number of threads you want to use, results in an integer
 * value, it should be possible to use it.
 */

#define number_of_threads 8 // Tested with 1, 2, 4 and 8 threads

extern pthread_t g_thread[number_of_threads];
extern int g_thread_aliveness[number_of_threads];

void *thandler(void *ptr);
void cleanup_thread_handler(void *p);

/*
 * The struct threaddata holds start parameters for the mandelbrot section
 * and the parameters thread specific start and stop parameters
 * for generating parts of the image.
 */

struct threaddata
{
  unsigned char *buffer;           // the pointer to the imagebuffer
  unsigned char (*colpalette)[3];  // the pointer to the colorpalette
  double xp;                       // start value of the mandelbrot section
  double yp;                       // start value of the mandelbrot section
  double xmin;                     // start value of the mandelbrot section
  double xmax;                     // start value of the mandelbrot section
  double ymin;                     // start value of the mandelbrot section
  double ymax;                     // start value of the mandelbrot section
  double zoom;                     // start value of the mandelbrot section
  int xy;                          // start pixel for writing to the imagebuffer
  int start_y;                     // start row for calculating the image
  int stop_y;                      // stop row for calculating the image
  int *am_I_alive;
};

#endif
