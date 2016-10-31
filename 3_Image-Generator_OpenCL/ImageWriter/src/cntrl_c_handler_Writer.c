/*
 * FILE =  /src/cntrl_c_handler_Writer.c
 *
 * This file holds the function that will be delivered to the signal handler
 * It invokes the cleanupW() defined in /src/cleanupW.c
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdlib.h>
#include "cleanupWriter.h"

void cntrl_c_handler_W(int signum)
{
  cleanupW();
  exit(EXIT_SUCCESS);
}
