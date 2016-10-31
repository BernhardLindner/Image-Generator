/*
 * FILE =  /src/cleanupWriter.c
 *
 * This function frees allocated memory segments, detaches the shared memory
 * segment and closes open imagefiles.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "global_ids_W.h"

void cleanupW(void)
{
  if (g_buffer != NULL)
  {
    free(g_buffer);
    g_buffer = NULL;
  }
  if (g_membuf != NULL)
  {
    if (shmdt(g_membuf) < 0)
    {
      perror("shmdt");
    }
  }
  if (g_name != NULL)
  {
    free(g_name);
    g_name = NULL;
  }
  if (g_pIMAGE != NULL)
  {
    if (fclose(g_pIMAGE) == 0)
    {
      g_pIMAGE = NULL;
    }
    else
    {
      printf("Error: IMAGE File could not be closed.\n");
    }
  }
}
