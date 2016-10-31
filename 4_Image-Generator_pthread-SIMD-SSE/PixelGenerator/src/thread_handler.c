/*
 * FILE = /src/thread_handler.c
 *
 * RELATED FILES:     *.c                              *.h
 * 									  numberOfPixel.c					         numberOfPixel.h
 * 										install_signal_handler.c				 install_signal_handler.h
 * 										interrupt_handler.c              interrupt_handler.h
 * 										cleanup_thread_handler.c         cleanup_thread_handler.h
 * 											 															 global_ids.h
 * 											 															 thread_handler.h
 * 											 															 universalSettings.h
 *
 * The thandler function is the real image generating function.
 * The folowing code is an adaption of the pseudo code to generate an image
 * of a section of the Mandelbrot Set.
 * https://en.wikipedia.org/wiki/Mandelbrot_set
 *
 * the struct threaddata is defined in the file thread_handler.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>

#include "numberOfPixel.h"
#include "global_ids.h"
#include "thread_handler.h"
#include "interrupt_handler.h"
#include "universalSettings.h"
#include "install_signal_handler.h"
#include "cleanup_thread_handler.h"

#include "xmmintrin.h"
#include "emmintrin.h"


void *thandler(void *ptr)
{

/*
 * pthread_cleanup_push() is a pthread function which set a cleanup handler.
 * The cleanup handler will be invoked when the thread exits.
 *
 * cleanup_thread_handler() is defined in cleanup_thread_handler.c.
 * As there is nothing to cleanup from inside the cleanup handler serves solely
 * for debugging the code.
 */

  pthread_cleanup_push(cleanup_thread_handler, NULL);

/*---------------------------------------------------------------------------*/
/* I N S T A L L  I N T E R R U P T   H A N D L E R                          */
/*---------------------------------------------------------------------------*/

/*
 * The init_signal_handler() function is defind in install_signal_handler.c
 * It takes a SIGNAL and a signal handler callback funciton as arguments.
 *
 * The interrupt_handler() function is defined in interrupt_handler.c
 * If a SIGUSR1 signal is sent by the cleanup() function (see cleanup.c)
 * this function invokes the interrupt_handler(), which then terminates
 * the thread.
 */

  if (init_signal_handler(SIGUSR1, interrupt_handler) == EXIT_FAILURE)
  {
    printf("Error Installing interrupt Handler\n");
  }

/*---------------------------------------------------------------------------*/
/* M A N D E L B R O T  S E T                                                */
/*---------------------------------------------------------------------------*/

  const int MAX_ITERATION = 1023;

  struct threaddata *hdata;
  hdata = (struct threaddata *) ptr;

/*
 * *hdata->am_I_alive is a pointer to the global
 * g_thread_aliveness[number_of_threads] variable. It is used to check if a
 * thread is active and needs to be killed by the cntrl_c_handler.
 * See cleanup.c for more detail.
 */

  (*hdata->am_I_alive) = 0;

/*
 * The follwing section contains Intel Intrinsics instructions for SIMD SSE
 * The Intel Intrinsics Guide provides detailed information on below used
 * funtions.
 * https://software.intel.com/sites/landingpage/IntrinsicsGuide/
 */

/*
 * __m128d one = _mm_set1_pd(double a); could be used instead of
 * __m128d one = _mm128_set_pd(a, a);
 */

  __m128d one = _mm_set_pd(1, 1);

  for (int pixel_y = hdata->start_y; pixel_y < hdata->stop_y; pixel_y++)
  {
    double h1y0;
    h1y0 = ((hdata->ymin + (pixel_y * hdata->yp)) / hdata->zoom);

    __m128d y0 = _mm_set_pd(h1y0, h1y0);
    __m128d h3y0 = _mm_mul_pd(y0, y0);

    for (int pixel_x = 0; pixel_x < WIDTH; pixel_x = pixel_x + 2)
    {
      double x10 = ((hdata->xmin + (pixel_x * hdata->xp)) / hdata->zoom);
      double x20 = ((hdata->xmin + ((pixel_x + 1) * hdata->xp)) / hdata->zoom);

      __m128d x0 = _mm_set_pd(x10, x20);

      __m128d x = _mm_set_pd(0, 0);

      __m128d y = _mm_set_pd(0, 0);

      __m128d rememberiteration = _mm_set_pd(0, 0);

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

/*
 * In order to get a speed up in calculation by using
 * "Cardioid and bulb checking" both parallel processed pixels need to fullfill
 * the condition:
 *
 * if (((q * (q + (x0 - 0.25))) < (0.25 * (y0 * y0))) ||
 *    (((x0 + 1) * (x0 + 1) + (y0 * y0)) < (0.0625)))
 *
 * If < 2 pixels meet the condition both of them will still need to go through
 * the long calculation process.
 */

/*******************************************************************************
 *  q = (x0 - 0.25) * (x0 - 0.25) + (y0 * y0);
 */

      __m128d h1 = _mm_set_pd(0.25, 0.25);
      __m128d h2 = _mm_set_pd(0, 0);
      h2 = _mm_sub_pd(x0, h1);
      __m128d h3 = _mm_set_pd(0, 0);
      h3 = _mm_mul_pd(h2, h2);
      __m128d q = _mm_add_pd(h3, h3y0);

/*
 *
 ******************************************************************************/

/*******************************************************************************
 * if (((q * (q + (x0 - 0.25))) < (0.25 * (y0 * y0))) ||
 *    (((x0 + 1) * (x0 + 1) + (y0 * y0)) < (0.0625)))
 * {
 *   iteration = MAX_ITERATION;
 *   hdata->buffer[hdata->xy] = hdata->colpalette[iteration][0];
 *   hdata->xy++;
 *   hdata->buffer[hdata->xy] = hdata->colpalette[iteration][1];
 *   hdata->xy++;
 *   hdata->buffer[hdata->xy] = hdata->colpalette[iteration][2];
 *   hdata->xy++;
 *   continue;
 * }
 */

      __m128d h1q = _mm_set_pd(0, 0);
      h1q = _mm_add_pd(q, h2);
      __m128d h2q = _mm_set_pd(0, 0);
      h2q = _mm_mul_pd(q, h1q);
      __m128d h3q = _mm_set_pd(0, 0);
      h3q = _mm_mul_pd(h1, h3y0);

      __m128d h1x0 = _mm_set_pd(1, 1);
      __m128d h2x0 = _mm_set_pd(0, 0);
      h2x0 = _mm_add_pd(x0, h1x0);
      __m128d h3x0 = _mm_set_pd(0, 0);
      h3x0 = _mm_mul_pd(h2x0, h2x0);
      __m128d h4x0 = _mm_set_pd(0, 0);
      h4x0 = _mm_add_pd(h3x0, h3y0);
      __m128d h5 = _mm_set_pd(0.0625, 0.0625);

/*
 * __m128d _mm_cmplt_pd(__m128d a, __m128d b)
 * Compare packed double-precision (64-bit) floating-point elements in a and b
 * for less-than.
 * a < b ?  0xFFFFFFFFFFFFFFFF : 0
 *
 * int _mm_movemask_pd(__m128d a)
 * Will set the two least significant bits of the return value (int)
 * corresponding to the most significant bit of the two elements of a.
 * If a1 = 0xFFFFFFFFFFFFFFFF and a0 = 0 the return value will be 2. [0010]
 * If a1 = 0 and a0 = 0xFFFFFFFFFFFFFFFF the return value will be 1. [0001]
 * If a1 = 0xFFFFFFFFFFFFFFFF and a0 = 0xFFFFFFFFFFFFFFFF the return value will
 * be 3. [0011]
 *
 * https://software.intel.com/sites/landingpage/IntrinsicsGuide/
 */

      if ((_mm_movemask_pd(_mm_cmplt_pd(h2q, h3q)) == 3) ||
          (_mm_movemask_pd(_mm_cmplt_pd(h4x0, h5)) == 3))
      {
        for (int c = 0; c < 2; c++)
        {
          hdata->buffer[hdata->xy] = 0;
          hdata->xy++;
          hdata->buffer[hdata->xy] = 0;
          hdata->xy++;
          hdata->buffer[hdata->xy] = 0;
          hdata->xy++;
        }
        continue;
      }
/*
 *
 ******************************************************************************/

/*---------------------------------------------------------------------------*/
/* C A L C U L A T I N G  T H E  M A N D E L B R O T  S E T                  */
/*---------------------------------------------------------------------------*/

/*******************************************************************************
 *
 * while ((((x * x) + (y * y)) < 4) && (iteration < MAX_ITERATION))
 * {
 *   double xtemp;
 *   xtemp = ((x * x) - (y * y) + x0);
 *
 *   y = ((2 * x * y) + y0);
 *   x = xtemp;
 *
 *   iteration = iteration + 1;
 * }
 *
 */

      while ((iteration < MAX_ITERATION))
      {
        __m128d h1x = _mm_set_pd(0, 0);
        h1x = _mm_mul_pd(x, x);
        __m128d h1y = _mm_set_pd(0, 0);
        h1y = _mm_mul_pd(y, y);
        __m128d hxy = _mm_set_pd(0, 0);
        hxy = _mm_add_pd(h1x, h1y);
        __m128d h1xy = _mm_set_pd(4, 4);

/*
 * ((x * x) + (y * y)) < 4 ? c3 = 0xFFFFFFFFFFFFFFFF : 0
 */
        __m128d c3 = _mm_cmplt_pd(hxy, h1xy);

/*
 * To design the termination condition two other mandelbrot SIMD examples
 * have been very helpful:
 * https://github.com/skeeto/mandel-simd by Chris Wellons
 * and http://iquilezles.org/www/articles/sse/sse.htm by Inigo Quilez
 *
 * Processing two pixels at a time makes it necessary to keep track if one or
 * the other has already fallen out of the condition. As long as
 * ((x * x) + (y * y)) < 4 rememberiteration will be incremented by 1.
 * The calculation will continue as long as both pixels no longer meet the
 * condition. If one pixel has already fallen out of the condition but
 * the other one has not, the finsihed pixel will unnecessarily continue to be
 * calculated.
 *
 * _mm_and_pd() computes the bitwise AND of all elements.
 * If c3 = 0xFFFFFFFFFFFFFFFF -> c3 AND 0x0000000000000001 -> only the least
 * significant bit will be 1 all other bits will be set to zero.
 */

        __m128d c3h1 = _mm_and_pd(c3, one);
        rememberiteration = _mm_add_pd(c3h1, rememberiteration);

/*
 * If the most significant bit of both double values is 0 then
 * _mm_movemask_pd() returns 0;
 */

        if (_mm_movemask_pd(c3) == 0)
        {
          break;
        }

        __m128d temp1x = _mm_set_pd(0, 0);
        temp1x = _mm_sub_pd(h1x, h1y);

        __m128d temp2x = _mm_set_pd(0, 0);
        temp2x = _mm_add_pd(temp1x, x0);

        __m128d temp1y = _mm_set_pd(0, 0);
        temp1y = _mm_mul_pd(x, y);

        __m128d temph2y = _mm_set_pd(2, 2);

        __m128d temp2y = _mm_set_pd(0, 0);
        temp2y = _mm_mul_pd(temp1y, temph2y);

        __m128d temp3y = _mm_set_pd(0, 0);
        temp3y = _mm_add_pd(temp2y, y0);
        y = temp3y;
        x = temp2x;

        iteration = iteration + 1;
      }

/*
 *
 ******************************************************************************/

      double position[2];

/*
 * void _mm_storeh_pd(double* mem_addr, __m128d a)
 * Store the upper double-precision (64-bit) floating-point element from a into
 * memory.
 *
 * void _mm_storel_pd(double* mem_addr, __m128d a)
 * Store the lower double-precision (64-bit) floating-point element from a into
 * memory.
 *
 * https://software.intel.com/sites/landingpage/IntrinsicsGuide/
 */
      _mm_storeh_pd(&position[0], rememberiteration);
      _mm_storel_pd(&position[1], rememberiteration);

      int pos[2];
      pos[0] = position[0];
      pos[1] = position[1];

/*
 * Looking up colors for the current iteration in the colorpalette (generated
 * by the function create_color_palette()) and writing the R G B values of the
 * current computed pixel into the local imagebuffer.
 */

      for (int c = 0; c < 2; c++)
      {
        hdata->buffer[hdata->xy] = hdata->colpalette[pos[c]][0];
        hdata->xy++;
        hdata->buffer[hdata->xy] = hdata->colpalette[pos[c]][1];
        hdata->xy++;
        hdata->buffer[hdata->xy] = hdata->colpalette[pos[c]][2];
        hdata->xy++;
      }
    }
  }

  (*hdata->am_I_alive) = -1;

/*
 * (#include <pthread.h>)
 * void pthread_cleanup_pop(int execute);
 * The function removes the routine at the top of the stack of clean-up handlers
 * and optionally executes it if execute is nonzero.
 */

  pthread_cleanup_pop(1);

/*
 * (#include <pthread.h>)
 * void pthread_exit(void *retval);
 * The function terminates the thread. Its argument could be used to return
 * information from the thread.
 */

  pthread_exit(0);
}
