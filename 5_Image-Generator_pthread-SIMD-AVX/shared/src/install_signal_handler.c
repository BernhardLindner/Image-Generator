/*
 * FILE = /src/install_signal_handler.c
 *
 * The init_signal_handler() function takes a SIGNAL and a callback function
 * and initiates a signal handler.
 * This file is used by the imageWriter and pixelGenerator program.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "universalSettings.h"

int init_signal_handler(int signum, void (*handlerfunc)(int))
{
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));

  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  act.sa_handler = handlerfunc;

  #if OS_FEDORA
/*
 * sa_restorer is not part of struct sigaction on osx.
 */

  act.sa_restorer = NULL;
  #endif

  if ((sigaction(signum, &act, NULL)) < 0)
  {
    perror("sigaction");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
