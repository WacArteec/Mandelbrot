#include "../include/Naive.h"

void Naivemandelbrot(double creal, double cimage, int *iter)
{
    double zreal = 0.0, zimage = 0.0;

    while (*iter < MAX_ITER)
    {
        double zr2 = zreal * zreal;
        double zi2 = zimage * zimage;

        if (zr2 + zi2 > 10.0)
            break;

        double new_zr = zr2 - zi2 + creal;
        double new_zi = 2 * zreal * zimage + cimage;

        zreal = new_zr;
        zimage = new_zi;
        (*iter)++;
    }
}