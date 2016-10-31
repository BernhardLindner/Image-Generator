/*
 * FILE = /src/OpenCL_mem_cleanup.h
 *
 * The mem_cleanup_opencl() function releases OpenCL Memory Objects.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include "mem_cleanup_opencl.h"

void mem_cleanup_opencl(struct cl_mem_data *data)
{
  cl_int err;
  err = clReleaseMemObject(data->imgb);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to release memory object imgb!\n");
  }
  err = clReleaseMemObject(data->colpb);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to release memory object colpb!\n");
  }
  err = clReleaseProgram(data->program);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to delete program object!\n");
  }
  err = clReleaseKernel(data->kernel);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to delete kernel object!\n");
  }
  err = clReleaseCommandQueue(data->commands);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to delete command queue!\n");
  }
  err = clReleaseContext(data->context);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to delete context!\n");
  }
}
