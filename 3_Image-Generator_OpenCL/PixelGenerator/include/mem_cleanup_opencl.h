/*
 * FILE = HEADER: /include/OpenCL_mem_cleanup.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */


#ifndef _mem_cleanup_opencl_
#define _mem_cleanup_opencl_

#include <stdio.h>
#include <stdlib.h>
#include "setup_OpenCL.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif


void mem_cleanup_opencl(struct cl_mem_data *data);

#endif
