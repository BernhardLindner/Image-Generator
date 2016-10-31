__kernel void mandelbrot(__global unsigned char *imagebuffer,
                          double xmin,
                          double xmax,
                          double ymin,
                          double ymax,
                          double e,
                          double zoom,
                          __global unsigned char *palette,
                          int WIDTH,
                          int HEIGHT)
{
  const int MAX_ITERATION = 1023;

  double xp;
  xp = ((xmax - xmin) / WIDTH);
  double yp;
  yp = ((ymax - ymin) / HEIGHT);

  int pixel_y = get_global_id(0);
  int pixel_x = get_global_id(1);

  if ((pixel_y < HEIGHT) && (pixel_x < WIDTH))
  {
    double x0;
    x0 = ((xmin + (pixel_x * xp)) / zoom);

    double y0;
    y0 = ((ymin + (pixel_y * yp)) / zoom);

    double x;
    x = 0.0;

    double y;
    y = 0.0;

    int iteration;
    iteration = 0;

    double q;
    q = (x0 - 0.25) * (x0 - 0.25) + (y0 * y0);

    if (((q * (q + (x0 - 0.25))) < (0.25 * (y0 * y0))) ||
      (((x0 + 1) * (x0 + 1) + (y0 * y0)) < (0.0625)))
    {
      iteration = MAX_ITERATION;
      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x))] =
      palette[iteration * 3];

      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 1)] =
      palette[iteration * 3 + 1];

      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 2)] =
      palette[iteration * 3 + 2];
    }
    else
    {
      while ((((x * x) + (y * y)) < 4) && (iteration < MAX_ITERATION))
      {
        double xtemp;
        xtemp = ((x * x) - (y * y) + x0);

        y = ((2 * x * y) + y0);
        x = xtemp;
        iteration = iteration + 1;
      }
      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x))] =
      palette[iteration * 3];

      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 1)] =
      palette[iteration * 3 + 1];

      imagebuffer[(pixel_y * WIDTH * 3 + (3 * pixel_x) + 2)] =
      palette[iteration * 3 + 2];
    }
  }
}
