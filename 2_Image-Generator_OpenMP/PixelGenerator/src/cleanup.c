/*
 * FILE =  /src/cleanup.c
 *
 * This function frees allocated memory segments, detaches the shared memory
 * segment and removes semaphores and shared memory from the system.
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
#include <signal.h>
#include <errno.h>
#include "global_ids.h"
#include "universalSettings.h"

void cleanup(void)
{
  #if DEBUG

  printf("\nCleanup\n");

  #endif

  if (g_shmid != -1)
  {
    if ((shmctl(g_shmid, IPC_RMID, 0)) < 0)
    {
      perror("shmctl");
    }
    else
    {
      g_shmid = -1;
    }
  }
  if (g_semid != -1)
  {
    if ((semctl(g_semid, IPC_RMID, 0)) < 0)
    {
      perror("semctl");
    }
    else
    {
      g_semid = -1;
    }
  }
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
    else
    {
      g_membuf = NULL;
    }
  }
  #if DEBUG

  printf("\nCleanup completed.\n");

  #endif
}
