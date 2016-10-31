/*
 * FILE =  /src/cleanup_thread_handler.c
 *
 * The cleanup_thread_handler() function shows how a cleanup handler for
 * individual threads could be implemented. 
 * if needed. In this project the cleanup_thread_handler() is only used for
 * debugging.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include "universalSettings.h"

void cleanup_thread_handler(void *p)
{
  #if DEBUG

  printf("\nCleanup thread handler\n");

  #endif
}
