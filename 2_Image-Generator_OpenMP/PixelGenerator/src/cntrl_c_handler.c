/*
 * FILE = /src/cntrl_c_handler.c
 *
 * This file holds the function that will be delivered to the signal handler
 * It invokes the cleanup() defined in cleanup.c
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cleanup.h>
#include "global_ids.h"

void cntrl_c_handler(int signum)
{
  cleanup();
  exit(EXIT_SUCCESS);
}
