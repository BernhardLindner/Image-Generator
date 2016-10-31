/*
 * FILE = /src/mandelbrot.c
 *
 * RELATED FILES:     *.c                             *.h
 * 										thread_handler.c                thread_handler.h
 * 										numberOfPixel.c				          numberOfPixel.h
 * 											 														  global_ids.h
 *
 * This function takes a colorpalette created by the function
 * create_color_palette() and the unsigned char *pointer to a local imagebuffer
 * as arguments.
 * The generate_image function sets the start parameters for creating the
 * madelbrot set and alters them everytime the function gets invoked.
 *
 * Depending on the number of threads specified in thread_handler.h this
 * function can split the computation of one image on several threads.
 * e.g.: If number_of_threads in thread_handler.h is set to 4, every thread
 * generates a quarter of the image.
 *
 * the struct threaddata holds the start and stop parameters for each thread,
 * the pointer to the local imagebuffer and colorpalette.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include <pthread.h>
#include "numberOfPixel.h"
#include "global_ids.h"
#include "thread_handler.h"

int generate_image(unsigned char palette[][3], unsigned char *imagebuffer)
{

/*
 * int mandel_segment can be set to 1, 2 or 3 to zoom into three different
 * segments of the mandelbrot set.
 */

  int mandel_segment = 2;

/*
 * start parameter for the madelbrot set
 */

  static double xmin = -2.5;
  static double xmax = 1.5;
  static double ymin = -1.5;
  static double ymax = 1.5;
  static double zoom = 1;
  static double e = 1;

/*
 * generating start parameters depending on the number of threads that are
 * handed to each thread.
 */

  struct threaddata tdata[number_of_threads];

  for (int n = 0; n < number_of_threads; n++)
  {
    tdata[n].buffer = imagebuffer;
    tdata[n].colpalette = palette;
    tdata[n].xp = ((xmax - xmin) / WIDTH);
    tdata[n].yp = ((ymax - ymin) / HEIGHT);
    tdata[n].xmin = xmin;
    tdata[n].xmax = xmax;
    tdata[n].ymin = ymin;
    tdata[n].ymax = ymax;
    tdata[n].zoom = zoom;
    tdata[n].xy = (HEIGHT * WIDTH/number_of_threads * 3 * n);
    tdata[n].start_y = (n * HEIGHT/number_of_threads);
    tdata[n].stop_y = ((n + 1) * HEIGHT/number_of_threads);
    tdata[n].am_I_alive = &g_thread_aliveness[n];
  }

/*
 * (#include <pthread.h>)
 * int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
 *                    void *(*start_routine) (void *), void *arg);
 * The function starts the threads.
 */

  for (int t = 0; t < number_of_threads; t++)
  {
    if (pthread_create(&g_thread[t], NULL, thandler, &tdata[t]) != 0)
    {
      perror("pthread_create");
      return -1;
    }
  }

/*
 * #include <pthread.h>)
 * int pthread_join(pthread_t thread, void **retval);
 * The function waits for the specified thread to terminate.
 */

  for (int j = 0; j < number_of_threads; j++)
  {
    if (pthread_join(g_thread[j], NULL) != 0)
    {
      perror("pthread_join");
      return -1;
    }
  }

/*
 * altering the start parameter to zoom into the madelbrot set.
 */

  if (mandel_segment == 1)
  {
    xmin = xmin - 0.005 * e;
    xmax = xmax - 0.005 * e;
    ymin = ymin - 0.0062 * e;
    ymax = ymax - 0.0062 * e;
    zoom = zoom + 0.01 * e;
    e++;
  }

  if (mandel_segment == 2)
  {
    xmin = xmin - 0.014 * e;
    xmax = xmax - 0.014 * e;
    zoom = zoom + 0.01 * e;
    e++;
  }

  if (mandel_segment == 3)
  {
    xmin = xmin - 0.015 * e;
    xmax = xmax - 0.015 * e;
    zoom = zoom + 0.01 * e;
    e++;
  }

  return 0;
}
