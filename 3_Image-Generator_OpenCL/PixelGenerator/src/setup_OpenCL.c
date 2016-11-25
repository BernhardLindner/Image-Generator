/*
 * FILE = /src/setup_OpenCL.c
 *
 * RELATED FILES:     *.c                              *.h
 *                    numberOfPixel.c                  numberOfPixel.h
 *                    mem_cleanup_opencl.c             mem_cleanup_opencl.h
 *                                                     setup_OpenCL.h
 *                                                     universalSettings.h
 *
 * This function takes a colorpalette created by the function
 * create_color_palette() and the unsigned char *pointer to a local imagebuffer
 * as arguments.
 *
 * The generate_image function generates an image of the mandelbrot set
 * with the use of OpenCL.
 *
 * By changing COMPUTE_DEVICE defined in setup_OpenCL.h the image can be
 * calculated either on the CPU or if available on a GPU.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include "setup_OpenCL.h"
#include "numberOfPixel.h"
#include "universalSettings.h"
#include "mem_cleanup_opencl.h"

/*
 * The following sources are great starting points on OpenCL.
 *
 * A great article on OpenCL:
 * "A Gentle Introduction to OpenCL"
 * http://www.drdobbs.com/parallel/a-gentle-introduction-to-opencl/231002854
 *
 * A great lecture course on OpenCL:
 * "Hands On OpenCL"
 * https://handsonopencl.github.io
 */

/*---------------------------------------------------------------------------*/
/* K E R N E L S O U R C E S                                                 */
/*---------------------------------------------------------------------------*/

/*
 * the kernelsource can be found inside kernelsource_OpenCL/mandelbrot_openCL.cl
 *
 * Use the script:
 * https://github.com/UoB-HPC/SNAP_MPI_OpenCL/blob/master/src/stringify_opencl
 * to convert the .cl kernelsource into a char *kernelsource string.
 *
 * Alternatively the .cl file could be opened with fopen() and read into a
 * char *string with fread().
 */

char * kernelsource =
"__kernel void mandelbrot(__global unsigned char *imagebuffer,\n"
"                          double xmin,\n"
"                          double xmax,\n"
"                          double ymin,\n"
"                          double ymax,\n"
"                          double e,\n"
"                          double zoom,\n"
"                          __global unsigned char *palette,\n"
"                          int WIDTH,\n"
"                          int HEIGHT)\n"
"{\n"
"  const int MAX_ITERATION = 1023;\n"
"\n"
"  double xp;\n"
"  xp = ((xmax - xmin) / WIDTH);\n"
"  double yp;\n"
"  yp = ((ymax - ymin) / HEIGHT);\n"
"\n"
"  int pixel_y = get_global_id(0);\n"
"  int pixel_x = get_global_id(1);\n"
"\n"
"  if ((pixel_y < HEIGHT) && (pixel_x < WIDTH))\n"
"  {\n"
"    double x0;\n"
"    x0 = ((xmin + (pixel_x * xp)) / zoom);\n"
"\n"
"    double y0;\n"
"    y0 = ((ymax - (pixel_y * yp)) / zoom);\n"
"\n"
"    double x;\n"
"    x = 0.0;\n"
"\n"
"    double y;\n"
"    y = 0.0;\n"
"\n"
"    int iteration;\n"
"    iteration = 0;\n"
"\n"
"    double q;\n"
"    q = (x0 - 0.25) * (x0 - 0.25) + (y0 * y0);\n"
"\n"
"    if (((q * (q + (x0 - 0.25))) < (0.25 * (y0 * y0))) ||\n"
"      (((x0 + 1) * (x0 + 1) + (y0 * y0)) < (0.0625)))\n"
"    {\n"
"      iteration = MAX_ITERATION;\n"
"      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x))] =\n"
"      palette[iteration * 3];\n"
"\n"
"      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 1)] =\n"
"      palette[iteration * 3 + 1];\n"
"\n"
"      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 2)] =\n"
"      palette[iteration * 3 + 2];\n"
"    }\n"
"    else\n"
"    {\n"
"      while ((((x * x) + (y * y)) < 4) && (iteration < MAX_ITERATION))\n"
"      {\n"
"        double xtemp;\n"
"        xtemp = ((x * x) - (y * y) + x0);\n"
"\n"
"        y = ((2 * x * y) + y0);\n"
"        x = xtemp;\n"
"        iteration = iteration + 1;\n"
"      }\n"
"      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x))] =\n"
"      palette[iteration * 3];\n"
"\n"
"      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 1)] =\n"
"      palette[iteration * 3 + 1];\n"
"\n"
"      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 2)] =\n"
"      palette[iteration * 3 + 2];\n"
"    }\n"
"  }\n"
"}\n"
;

int setup_OpenCL(unsigned char *palette, void *OpenCLdata)
{

/*
 * struct cl_mem_data defined in mandelbrot.h
 */

  struct cl_mem_data *data = (struct cl_mem_data *) OpenCLdata;

/*---------------------------------------------------------------------------*/
/* I D E N T I F Y  A  P L A T F O R M                                       */
/*---------------------------------------------------------------------------*/

  cl_int err;
  cl_platform_id platform;

  err = clGetPlatformIDs(1, &platform, NULL);
  if (err < 0)
  {
    perror("Couldn't identify a platform");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* C O N N E C T  T O  A  C O M P U T E   D E V I C E                        */
/*---------------------------------------------------------------------------*/

/*
 * COMPUTE_DEVICE defined in mandelbrot.h
 */

  cl_device_id device;
  cl_device_id devices[3];
  cl_uint numdevices = 3;

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 3, devices, &numdevices);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }

  device = devices[COMPUTE_DEVICE];

  char devicename[256];
  cl_device_info info = CL_DEVICE_NAME;
  err = clGetDeviceInfo(device, info, 256, devicename, NULL);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to get Device Info!\n");
    return EXIT_FAILURE;
  }

  #if DEBUG

  printf("\nUsing OpenCL device: %s\n", devicename);

  #endif

/*---------------------------------------------------------------------------*/
/* C R E A T E  A  C O M P U T E  C O N T E X T                              */
/*---------------------------------------------------------------------------*/

  data->context = clCreateContext(0, 1, &device, NULL, NULL, &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create a compute context!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* C R E A T E  A  C O M M A N D  Q U E U E                                  */
/*---------------------------------------------------------------------------*/

  /* OpenCL 2 */
  #if OS_FEDORA

  data->commands = clCreateCommandQueueWithProperties(data->context, device,
                                                     0, &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create a command commands!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

  /* OpenCL 1.2 */
  #else

  data->commands = clCreateCommandQueue(data->context, device, 0, &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create a command commands!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

  #endif

/*---------------------------------------------------------------------------*/
/* C R E A T E  M E M O R Y  B U F F E R S                                   */
/*---------------------------------------------------------------------------*/

  data->imgb = clCreateBuffer(data->context, CL_MEM_READ_WRITE,
                             sizeof(unsigned char) * MAX_DATA, NULL, &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: creating Buffer for imagedata!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

  data->colpb = clCreateBuffer(data->context, CL_MEM_READ_WRITE,
                              sizeof(unsigned char) * 3072, NULL, &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: creating Buffer for colorpalette!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* C R E A T E  A  P R O G R A M  F R O M  K E R N E L S O U R C E           */
/*---------------------------------------------------------------------------*/

  data->program = clCreateProgramWithSource(data->context, 1,
                                           (const char **) &kernelsource,
                                            NULL, &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: Creating program from kernelsource!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* B U I L D  T H E  P R O G R A M  E X E C U T A B L E                      */
/*---------------------------------------------------------------------------*/

  err = clBuildProgram(data->program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    size_t len;
    char buffer[2048];

    printf("Error: Failed to build program executable!\n");
    clGetProgramBuildInfo(data->program, device, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* C R E A T E  T H E  C O M P U T E  K E R N E L                            */
/*---------------------------------------------------------------------------*/

  data->kernel = clCreateKernel(data->program, "mandelbrot", &err);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create compute kernel!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* W R I T E  C O L O R P A L E T T E  I N T O  B U F F E R                  */
/*---------------------------------------------------------------------------*/

  err = clEnqueueWriteBuffer(data->commands, data->colpb, CL_TRUE, 0,
                             sizeof(unsigned char) * 3072, palette,
                             0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to write colorpalette to buffer!\n");
    mem_cleanup_opencl(data);
    return EXIT_FAILURE;
  }

  /*---------------------------------------------------------------------------*/
  /* S E T  K E R N E L  A R G U M E N T S                                     */
  /*---------------------------------------------------------------------------*/

    err =  clSetKernelArg(data->kernel, 0, sizeof(cl_mem), &data->imgb);
    err |= clSetKernelArg(data->kernel, 7, sizeof(cl_mem), &data->colpb);
    err |= clSetKernelArg(data->kernel, 8, sizeof(int), &WIDTH);
    err |= clSetKernelArg(data->kernel, 9, sizeof(int), &HEIGHT);

    if (err != CL_SUCCESS)
    {
      printf("Error: Failed to set kernel arguments! %d\n", err);
      mem_cleanup_opencl(data);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
