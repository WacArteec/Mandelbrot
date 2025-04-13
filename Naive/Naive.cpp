#include "Naive.h"

int MandelbrotNaive(double creal, double cimage)
{
    double zreal = 0.0, zimage = 0.0;
    int iter = 0;

    while (iter < MAX_ITER)
    {
        double zr2 = zreal * zreal;
        double zi2 = zimage * zimage;

        if (zr2 + zi2 > 10.0)
            break;

        double new_zr = zr2 - zi2 + creal;
        double new_zi = 2 * zreal * zimage + cimage;

        zreal = new_zr;
        zimage = new_zi;
        iter++;
    }

    return iter;
}

void Naive(double *real, double *imag, int* iters)
{
    for (int i = 0; i < 4; i++)
        iters[i] = MandelbrotNaive(real[i], imag[i]);

}