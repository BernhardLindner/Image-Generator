/*
 * FILE = HEADER: /include/madelbrot.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */


#ifndef _mandelbrot_
#define _mandelbrot_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

/*
 * COMPUTE_DEVICE can be set to 0 = CPU, 1 = integrated GPU, 2 = dedicated GPU
 */

#define COMPUTE_DEVICE 0

struct cl_mem_data
{
  cl_mem           imgb;
  cl_mem           colpb;
  cl_context       context;
  cl_command_queue commands;
  cl_program       program;
  cl_kernel        kernel;
};

int generate_image(unsigned char *palette, unsigned char *imagebuffer);

#endif
