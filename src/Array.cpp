#include "../include/Array.h"

void ArrayMandelbrot(double *creal, double *cimag, int *iters)
{
    double zreal[4] = {0.0, 0.0, 0.0, 0.0};
    double zimag[4] = {0.0, 0.0, 0.0, 0.0};
    
    int active[4] = {1, 1, 1, 1};

    for (int i = 0; i < 4; i++)
        iters[i] = 0;

    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        int any_active = 0;

        double zr2[4], zi2[4];

        for (int i = 0; i < 4; i++)
        {
            if (!active[i])
                continue;

            zr2[i] = zreal[i] * zreal[i];
            zi2[i] = zimag[i] * zimag[i];
        }

        for (int i = 0; i < 4; i++)
        {
            if (!active[i])
                continue;

            if (zr2[i] + zi2[i] > 10.0)
            {
                active[i] = 0;
                continue;
            }

            any_active = 1;
        }

        if (!any_active)
            break;

        for (int i = 0; i < 4; i++)
        {
            if (!active[i])
                continue;

            double new_zr = zr2[i] - zi2[i] + creal[i];
            double new_zi = 2 * zreal[i] * zimag[i] + cimag[i];

            zreal[i] = new_zr;
            zimag[i] = new_zi;

            iters[i]++;
        }
    }
}