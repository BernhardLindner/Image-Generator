/*
 * FILE = HEADER: /include/global_ids.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#ifndef _global_ids_
#define _global_ids_

#include <stdio.h>
#include "setup_OpenCL.h"

int g_shmid;
int g_semid;
unsigned char *g_buffer;
unsigned char *g_membuf;
struct cl_mem_data g_data;

#endif
