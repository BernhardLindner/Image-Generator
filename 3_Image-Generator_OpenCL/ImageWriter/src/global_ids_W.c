/*
 * FILE =  /src/global_ids_W.c
 *
 * This file holds variables that need to be accessed by the SIGINT handler.
 *
 * Copyright (c) 2017 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include "global_ids_W.h"
#include <stdio.h>

int g_shmid;
int g_semid;
unsigned char *g_buffer;
unsigned char *g_membuf;
char *g_name;
FILE *g_pIMAGE;
