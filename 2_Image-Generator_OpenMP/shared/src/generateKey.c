/*
 * FILE = /src/generateKey.c
 *
 * This file holds the function that generates a key, needed for retrieving
 * a shared memory segment and semaphores.
 * This file - together with generateKey.h - is used by the pixelGenerator and
 * imageWriter program.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <sys/ipc.h>
#include <stdio.h>

key_t generateKey(void)
{
  key_t key;
  key = ftok("/etc", 'b');
  if (key == -1)
  {
    perror("ftok");
  }
  return key;
}
