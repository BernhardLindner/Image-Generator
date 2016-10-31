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
#include <pthread.h>
#include "global_ids.h"
#include "thread_handler.h"
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
  for (int j = 0; j < number_of_threads; j++)
  {
    #if DEBUG

    printf("%d\n", g_thread_aliveness[j]);

    #endif

/*
 * Having installed the signal handler for SIGINT in the main thread,
 * the signal, sent by pressing ctrl-c, interrupts (blocks) only the main
 * thread. The threads calculating the values for each pixel of the mandlebrot
 * set will continue executing until they have finished their task or the
 * program exits.
 * Thank you to Christian Fibich for pointing this out!
 *
 * In order to be able to free the memory of the local imagebuffer no thread
 * is allowed to access this memory segment anymore. So all running threads
 * which are potentially accessing this memory need to be killed.
 *
 * If a signal gets send to a thread which has already terminated the behavior
 * of the pthread_kill() function is undefind.
 * Therfore g_thread_aliveness is used to check if a thread is still alive.
 * If the respective thread is still executing a signal is send which will
 * invoke the interrupt_handler() function of the thread which then terminates
 * the thread.
 *
 * sending SIGTERM or SIGKILL via pthread_kill() to the respective thread
 * shuts down the whole program before beeing able to free allocated memory.
 */

    if (g_thread_aliveness[j] == 0)
    {
      if (pthread_kill(g_thread[j], SIGUSR1) != 0)
      {
        if (errno == ESRCH)
        {
          printf("\nThread has already terminated.\n");
        }
        else
        {
          perror("pthread_kill");
        }
      }
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
