/*
 * FILE =  interrupt_handler.c
 *
 * The interrupt_handler invokes pthread_exit() to terminate the running thread.
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
#include "global_ids.h"
#include "thread_handler.h"
#include "universalSettings.h"
#include <pthread.h>

void interrupt_handler(int signum)
{
  #if DEBUG

  printf("\ninterrupt_handler\n");

  #endif

  pthread_exit(0);
}
