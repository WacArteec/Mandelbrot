#ifndef AVX_H
#define AVX_H

#include <immintrin.h>
#include "enum.h"

void AVXMandelbrot(double *real, double *imag, int* iters);

#endif //AVX_H