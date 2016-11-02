/*
 * FILE = /src/thread_handler.c
 *
 * RELATED FILES:     *.c                              *.h
 *                    numberOfPixel.c                  numberOfPixel.h
 *                    install_signal_handler.c         install_signal_handler.h
 *                    interrupt_handler.c              interrupt_handler.h
 *                    cleanup_thread_handler.c         cleanup_thread_handler.h
 *                                                     global_ids.h
 *                                                     thread_handler.h
 *                                                     universalSettings.h
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
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "global_ids.h"
#include "numberOfPixel.h"
#include "thread_handler.h"
#include "interrupt_handler.h"
#include "universalSettings.h"
#include "install_signal_handler.h"
#include "cleanup_thread_handler.h"

void *thandler(void *ptr)
{

/*
 * (#include <pthread.h>)
 * void pthread_cleanup_push(void (*routine)(void *),void *arg);
 * The function pushes routine onto the top of the stack of clean-up handlers.
 * When routine is later invoked, it will be given arg as its argument.
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

  struct threaddata *hdata;
  hdata = (struct threaddata *) ptr;

/*
 * *hdata->am_I_alive is a pointer to the global
 * g_thread_aliveness[number_of_threads] variable. It is used to check if a
 * thread is active and needs to be killed by the cntrl_c_handler.
 * See cleanup.c for more details.
 */

  (*hdata->am_I_alive) = 0;

  const int MAX_ITERATION = 1023;

  for (int pixel_y = hdata->start_y; pixel_y < hdata->stop_y; pixel_y++)
  {
    double y0;
    y0 = ((hdata->ymin + (pixel_y * hdata->yp)) / hdata->zoom);

    for (int pixel_x = 0; pixel_x < WIDTH; pixel_x++)
    {
      double x0;
      x0 = ((hdata->xmin + (pixel_x * hdata->xp)) / hdata->zoom);

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
        hdata->buffer[hdata->xy] = hdata->colpalette[iteration][0];
        hdata->xy++;
        hdata->buffer[hdata->xy] = hdata->colpalette[iteration][1];
        hdata->xy++;
        hdata->buffer[hdata->xy] = hdata->colpalette[iteration][2];
        hdata->xy++;
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

/*
 * Looking up colors for the current iteration in the colorpalette (generated
 * by the function create_color_palette()) and writing the R G B values of the
 * current computed pixel into the local imagebuffer.
 */

      hdata->buffer[hdata->xy] = hdata->colpalette[iteration][0];
      hdata->xy++;
      hdata->buffer[hdata->xy] = hdata->colpalette[iteration][1];
      hdata->xy++;
      hdata->buffer[hdata->xy] = hdata->colpalette[iteration][2];
      hdata->xy++;
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
