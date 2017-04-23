/*
 * FILE =  /src/global_ids.c
 *
 * This file holds variables that need to be accessed by the SIGINT handler.
 *
 * Copyright (c) 2017 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include "global_ids.h"
#include "setup_OpenCL.h"
#include <stdio.h>

int g_shmid;
int g_semid;
unsigned char *g_buffer;
unsigned char *g_membuf;
struct cl_mem_data g_data;
