/*
 * FILE = /src/colorpalette.c
 *
 * The create_color_palette function takes three of my "favorite" colors
 * and fills up an array of 1024 by 3 to generate sort of a color gradient.
 * 1024 stands for the number of iterations specified for the generation
 * of the mandelbrot set (see thread_handler.c)
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#include <stdio.h>

int create_color_palette(unsigned char palette[][3])
{
  palette[0][0] = 10;  // R
  palette[0][1] = 5;   // G
  palette[0][2] = 100; // B

  int a = 10;
  int b = 5;
  int c = 100;
  int q = 1;

  for (int p = 1; p < 100; p++)
  {
    palette[p][0] = (a + 2 * q);
    palette[p][1] = (b + 1 * q);
    palette[p][2] = (c - 1 * q);
    q++;
  }

  palette[100][0] = 200;
  palette[100][1] = 50;
  palette[100][2] = 25;

  int d = 200;
  int e = 50;
  int f = 25;
  q = 1;

  for (int p = 101; p < 200; p++)
  {
    palette[p][0] = d;
    palette[p][1] = (e + 2 * q);
    palette[p][2] = (f + 2 * q);
    q++;
  }

  palette[200][0] = 215;
  palette[200][1] = 198;
  palette[200][2] = 150;

  for (int p = 201; p < 400; p++)
  {
    palette[p][0] = 216;
    palette[p][1] = (199 - p + 20);
    palette[p][2] = (216 - p);
  }

  for (int p = 201; p < 400; p++)
  {
    palette[p][0] = (216 - p);
    palette[p][1] = 20;
    palette[p][2] = (16 + p + 10);
  }

  for (int p = 400; p < 600; p++)
  {
    palette[p][0] = (16 + p + 10);
    palette[p][1] = (20 + p + 20);
    palette[p][2] = (255 - p);
  }

  for (int p = 600; p < 800; p++)
  {
    palette[p][0] = (201 - p);
    palette[p][1] = (230 - p);
    palette[p][2] = (50 + p);
  }

  for (int p = 800; p < 1023; p++)
  {
    palette[p][0] = (0 + p);
    palette[p][1] = (27 + p);
    palette[p][2] = 188;
  }

  palette[1023][0] = 0;
  palette[1023][1] = 0;
  palette[1023][2] = 0;
  return 0;
}
