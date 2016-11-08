/*
 * FILE = /src/mandelbrot.c
 *
 * RELATED FILES:     *.c                              *.h
 *                    numberOfPixel.c                  numberOfPixel.h
 *
 * This function takes a colorpalette created by the function
 * create_color_palette() and the unsigned char *pointer to a local imagebuffer
 * as arguments.
 *
 * The generate_image function uses OpenMP to generate the mandelbrot set.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include "numberOfPixel.h"

/*
 * A great introduction to OpenMP:
 * "Guide into OpenMP: Easy multithreading programming for C++"
 * http://bisqwit.iki.fi/story/howto/openmp/
 */

#define OPENMP 1

#if OPENMP
  #include <omp.h>
#endif

int generate_image(unsigned char palette[][3], unsigned char *imagebuffer)
{

/*
 * start parameter;
 */

  const int MAX_ITERATION = 1023;
  static double xmin = -2.5;
  static double xmax = 1.5;
  static double ymin = -1.5;
  static double ymax = 1.5;

  int mandel_segment = 2;

  static double e = 1;

  static double zoom = 1;
  double xp;
  xp = ((xmax - xmin) / WIDTH);
  double yp;
  yp = ((ymax - ymin) / HEIGHT);


  #if OPENMP
  static int numthreads = 0;
  if (numthreads == 0)
  {
    numthreads = omp_get_max_threads();
    printf("%d threads\n", numthreads);
  }
  #pragma omp parallel for
  #endif

  for (int pixel_y = 0; pixel_y < HEIGHT; pixel_y++)
  {
    for (int pixel_x = 0; pixel_x < WIDTH; pixel_x++)
    {
      double x0;
      x0 = ((xmin + (pixel_x * xp)) / zoom);

      double y0;
      y0 = ((ymax - (pixel_y * yp)) / zoom);

      double x;
      x = 0.0;

      double y;
      y = 0.0;

      int iteration;
      iteration = 0;

/*---------------------------------------------------------------------------*/
/* C A R D I O I D  A N D  B U L B  C H E C K I N G                          */
/*---------------------------------------------------------------------------*/

/*
 * Cardioid and bulb checking:
 * https://en.wikipedia.org/wiki/Mandelbrot_set
 * "One way to improve calculations is to find out beforehand whether the given
 * point lies within the cardioid or in the period-2 bulb."
 */
      double q;
      q = (x0 - 0.25) * (x0 - 0.25) + (y0 * y0);

      if (((q * (q + (x0 - 0.25))) < (0.25 * (y0 * y0))) ||
         (((x0 + 1) * (x0 + 1) + (y0 * y0)) < (0.0625)))
      {
        iteration = MAX_ITERATION;
        imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x))] =
                    palette[iteration][0];
        imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 1)] =
                    palette[iteration][1];
        imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 2)] =
                    palette[iteration][2];
        continue;
      }

/*---------------------------------------------------------------------------*/
/* C A L C U L A T I N G  T H E  M A N D E L B R O T  S E T                  */
/*---------------------------------------------------------------------------*/

      while ((((x * x) + (y * y)) < 4) && (iteration < MAX_ITERATION))
      {
        double xtemp;
        xtemp = ((x * x) - (y * y) + x0);

        y = ((2 * x * y) + y0);
        x = xtemp;
        iteration = iteration + 1;
      }
      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x))] =
                  palette[iteration][0];
      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 1)] =
                  palette[iteration][1];
      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 2)] =
                  palette[iteration][2];
    }
  }

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
