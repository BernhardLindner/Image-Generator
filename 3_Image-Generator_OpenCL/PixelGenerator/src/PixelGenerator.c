/*
 * FILE = MAIN:       /src/PixelGenerator.c
 *
 * RELATED FILES:     *.c                              *.h
 *                    generateKey.c                    generateKey.h
 *                    numberOfPixel.c                  numberOfPixel.h
 *                    cleanup.c                        cleanup.h
 *                    cntrl_c_handler.c                cntrl_c_handler.h
 *                    colorpalette.c                   colorpalette.h
 *                    mandelbrot.c                     mandelbrot.h
 *                    install_signal_handler.c         install_signal_handler.h
 *                    mem_cleanup_opencl.c             mem_cleanup_opencl.h
 *                                                     global_ids.h
 *                                                     universalSettings.h
 *
 * DEPENDS ON:        imageWriter program
 *
 * A program that generates images of the mandelbrot set and writes them to a
 * shared memory segment. It depends on the imageWriter program to read the
 * image data out of the shared memory segment.
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#include "generateKey.h"
#include "global_ids.h"
#include "numberOfPixel.h"
#include "cntrl_c_handler.h"
#include "universalSettings.h"
#include "install_signal_handler.h"
#include "colorpalette.h"
#include "mandelbrot.h"
#include "cleanup.h"
#include "time.h"

#if OS_FEDORA

/*
 * necessary on Fedora
 * unecessary on OSX 10.9 -> redefinition of semun
 */

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
} semunion;

#endif

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    if ((strncmp(argv[1], "help", 4) == 0) || (strncmp(argv[1], "-h", 2) == 0))
    {
      printf("\nThis program generates an image of the mandlebrot set and\n"
             "writes the picture into a shared memory segmet. This program\n"
             "depends on the imageWriter program reading from the shared memory"
             "\nsegment and wirting the image into a .ppm file\n"
             "\nThis program does not take any cmdline arguments.\n\n");
      exit(EXIT_SUCCESS);
    }
    else
    {
      printf("\nThis program does not take any cmdline arguments.\n\n");
      exit(EXIT_FAILURE);
    }
  }

/*
 * Initalize values for global variables (declared in global_ids.h)
 * to determine if a segment should be freed or removed from inside
 * the cleanup() function.
 */

  g_shmid = -1;
  g_semid = -1;
  g_membuf = NULL;
  g_buffer = NULL;

/*---------------------------------------------------------------------------*/
/* I N S T A L L  S I G N A L  H A N D L E R                                 */
/*---------------------------------------------------------------------------*/

/*
 * The init_signal_handler() function is defind in install_signal_handler.c
 * It takes a SIGNAL and a signal handler callback funciton as arguments.
 *
 * The cntrl_c_handler function is defined in cntrl_c_handler.c
 * If a SIGINT signal is sent by pressing ctrl+c this function invokes the
 * cleanup() function and termiates the program.
 */

  if (init_signal_handler(SIGINT, cntrl_c_handler) == EXIT_FAILURE)
  {
    printf("Error Installing Singnal Handler\n");
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* G E N E R A T E  S H A R E D  M E M O R Y  S E G M E N T                  */
/*---------------------------------------------------------------------------*/

  key_t key = -1;

/*
 * The generateKey() functions is defined in generateKey.c
 * This function is used by the imageWriter and pixelGenerator to receive a key.
 */

  key = generateKey();
  if (key == -1)
  {
    printf("Error generating key\n");
    return EXIT_FAILURE;
  }

/*
 * Generating a shared memory segment for MAX_DATA (defined in numberOfPixel.c)
 */

  g_shmid = shmget(key, MAX_DATA, IPC_CREAT | 0600);
  if (g_shmid >= 0)
  {
    g_membuf = shmat(g_shmid, 0, 0);
    if (g_membuf == (unsigned char *) -1)
    {
      perror("shmat");
      cleanup();
      return EXIT_FAILURE;
    }
  }
  else
  {
    perror("shmget");
    cleanup();
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* G E N E R A T E  S E M A P H O R E S                                      */
/*---------------------------------------------------------------------------*/

  if ((g_semid = semget(key, 2, IPC_CREAT | 0600)) < 0)
  {
    perror("semget");
    cleanup();
    return EXIT_FAILURE;
  }
  struct sembuf s1;
  struct sembuf s2;

  union semun semunion;

/*
 * Setting start values for semaphore one and two.
 * The ImageWriter semaphore gets set to 0 at first which blocks the ImageWriter
 * program from reading data out of the shared memory segment.
 */

  semunion.val = 1;
  if ((semctl(g_semid, 0, SETVAL, semunion)) < 0)
  {
    perror("semctl");
    cleanup();
    return EXIT_FAILURE;
  }

  semunion.val = 0;
  if ((semctl(g_semid, 1, SETVAL, semunion)) < 0)
  {
    perror("semctl");
    cleanup();
    return EXIT_FAILURE;
  }

/*
 * prepare semaphore 1
 */
  s1.sem_num = 0;
  s1.sem_flg = SEM_UNDO;

/*
 * prepare semaphore 2
 */
  s2.sem_num = 1;
  s2.sem_flg = SEM_UNDO;

/*---------------------------------------------------------------------------*/
/* G E N E R A T E  C O L O R  P A L E T T E  &  I M A G E  B U F F E R      */
/*---------------------------------------------------------------------------*/


/*
 * Creating a very primitive colorpalette. The maximum value of iterations
 * for calculating the mandelbrot set (see thread_handler.c) is set to 1023
 * (starts at 0)
 * Each iteration gets assigned a color (RGB values).
 */

  static unsigned char PALETTE[1024][3];

  if (create_color_palette(PALETTE) != 0)
  {
    printf("Error creating colorpalette\n");
    cleanup();
    return EXIT_FAILURE;
  }

/*
 * Saving the colorpalette inside a onedimensional buffer to be able to copy it
 * to openCL memory.
 */

  static unsigned char palette_for_openCL[3072];
  int cp = 0;
  for (int i = 0; i < 1024; i++)
  {
    palette_for_openCL[cp] = PALETTE[i][0];
    cp++;
    palette_for_openCL[cp] = PALETTE[i][1];
    cp++;
    palette_for_openCL[cp] = PALETTE[i][2];
    cp++;
  }

/*
 * Generating a local image buffer where the image is stored before it is
 * written to the shared memory segment.
 */

  g_buffer = (unsigned char *) calloc(MAX_DATA, sizeof(unsigned char));
  if (g_buffer == NULL)
  {
    perror("calloc");
    cleanup();
    return EXIT_FAILURE;
  }

/*---------------------------------------------------------------------------*/
/* G E N E R A T E  I M A G E  D A T A                                       */
/*                                                                           */
/* W R I T E  I M A G E  T O  S H A R E D  M E M O R Y                       */
/*---------------------------------------------------------------------------*/

  #if TIMER_OUTPUT

  clock_t start;
  clock_t diff;
  int countimage = 1;

  #endif


  while (1)
  {

/*
 * TIMER_OUTPUT can be set to 1 = ON in universalSettings.c
 */

    #if TIMER_OUTPUT

    start = clock();

    #endif

/*
 * generate_image() (defined in mandelbrot.c) creates image data and writes it
 * to the local buffer.
 */

    if (generate_image(palette_for_openCL, g_buffer) == -1)
    {
      printf("Error generating image data\n");
      cleanup();
      return EXIT_FAILURE;
    }

    #if TIMER_OUTPUT

    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Time needed for image %d\t%d seconds %d milliseconds\n", countimage,
           msec/1000, msec%1000);
    countimage++;

    #endif

/*
 * lock semaphore 1
 */

    s1.sem_op = -1;

    if (semop(g_semid, &s1, 1) == -1)
    {
      perror("semop");
      cleanup();
      return EXIT_FAILURE;
    }

/*
 * Writing the local buffer to the shared memory segment
 */

    for (int i = 0; i < MAX_DATA; i++)
    {
        g_membuf[i] = g_buffer[i];
    }

/*
 * unlock semaphore 2
 */

    s2.sem_op = 1;

    if (semop(g_semid, &s2, 1) == -1)
    {
      perror("semop");
      cleanup();
      return EXIT_FAILURE;
    }
  }

/*
 * END OF while (1)
 */
  return EXIT_SUCCESS;
}
