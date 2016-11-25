/*
 * FILE = /src/generate_image.c
 *
 * RELATED FILES:     *.c                              *.h
 *                    numberOfPixel.c                  numberOfPixel.h
 *                    mem_cleanup_opencl.c             mem_cleanup_opencl.h
 *                                                     setup_OpenCL.h
 *                                                     universalSettings.h
 *                                                     generate_image.h
 *
 *
 * The generate_image function generates an image of the mandelbrot set
 * with the use of OpenCL.
 *
 * Executing the OpenCL kernel build inside the setup_OpenCL() function and
 * changing the start parameters of the mandelbrot set.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include "generate_image.h"
#include "numberOfPixel.h"
#include "universalSettings.h"
#include "mem_cleanup_opencl.h"

int generate_image(unsigned char *imagebuffer, void *OpenCLdata)
{

/*
 * struct cl_mem_data defined in mandelbrot.h
 */

  struct cl_mem_data *data = (struct cl_mem_data *) OpenCLdata;

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
  static double e = 1;
  static double zoom = 1;


/*---------------------------------------------------------------------------*/
/* S E T  K E R N E L  A R G U M E N T S                                     */
/*---------------------------------------------------------------------------*/

  cl_int err;
  err  = clSetKernelArg(data->kernel, 1, sizeof(double), &xmin);
  err |= clSetKernelArg(data->kernel, 2, sizeof(double), &xmax);
  err |= clSetKernelArg(data->kernel, 3, sizeof(double), &ymin);
  err |= clSetKernelArg(data->kernel, 4, sizeof(double), &ymax);
  err |= clSetKernelArg(data->kernel, 5, sizeof(double), &e);
  err |= clSetKernelArg(data->kernel, 6, sizeof(double), &zoom);

  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to set kernel arguments! %d\n", err);
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* E X E C U T E  T H E  K E R N E L                                         */
/*---------------------------------------------------------------------------*/

  const size_t global[2] = {HEIGHT, WIDTH};
  err = clEnqueueNDRangeKernel(data->commands, data->kernel, 2, NULL, global,
                               NULL, 0, NULL, NULL);
  if (err)
  {
    printf("Error: Failed to execute kernel!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*
 * Wait for the kernel to finish
 */

  err = clFinish(data->commands);
  if (err)
  {
    printf("Error: Waiting for commands to finish!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*
 * Write the calculated image back into the imagebuffer
 */

  err = clEnqueueReadBuffer(data->commands, data->imgb, CL_TRUE, 0,
                            sizeof(unsigned char) * MAX_DATA, imagebuffer,
                            0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to read image back into imagebuffer!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
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

  return EXIT_SUCCESS;
}
