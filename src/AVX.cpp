#include "../include/AVX.h"
#include <stdio.h>

void AVXMandelbrot(double *real, double *imag, int* iters)
{
    __m256d creal = _mm256_load_pd(real);
    __m256d cimage = _mm256_load_pd(imag);

    __m256d zreal = _mm256_setzero_pd();  // double zreal = 0.0, zimage = 0.0;
    __m256d zimage = _mm256_setzero_pd(); //
    __m256i iter = _mm256_setzero_si256();

    __m256d arr_of_ten = _mm256_set1_pd(10.0);
    __m256i arr_of_one = _mm256_set1_epi64x(1);
    __m256d arr_of_two = _mm256_set1_pd(2.0);

    for (int i = 0; i < MAX_ITER; i++)
    {
        __m256d zr2 = _mm256_mul_pd(zreal, zreal);   // double zr2 = zreal * zreal;
        __m256d zi2 = _mm256_mul_pd(zimage, zimage); // double zi2 = zimage * zimage;

        __m256d mask = _mm256_cmp_pd(_mm256_add_pd(zr2, zi2),
                                     arr_of_ten,
                                     _CMP_LE_OQ); // if (zr2 + zi2 <= 10.0) _CMP_LE_OQ - это <=

        __m256i add_iters = _mm256_castpd_si256(_mm256_and_pd(mask, _mm256_castsi256_pd(arr_of_one))); // у всех точек, что не выбыли, добавляем единицу к количеству циклов
        iter = _mm256_add_epi64(iter, add_iters);                                    //

        if (_mm256_testz_pd(mask, mask) == 1) // возвращаетя 1, если все нули
            break;

        __m256d new_zr = _mm256_add_pd(_mm256_sub_pd(zr2, zi2), creal);                            // double new_zr = zr2 - zi2 + creal;
        __m256d new_zi = _mm256_add_pd(_mm256_mul_pd(arr_of_two, _mm256_mul_pd(zreal, zimage)), cimage); // double new_zi = 2 * zreal * zimage + cimage;

        zreal = _mm256_blendv_pd(zreal, new_zr, mask);   // zreal = new_zr;
        zimage = _mm256_blendv_pd(zimage, new_zi, mask); // zimage = new_zi;
    }

    _mm256_storeu_si256((__m256i *)iters, iter);
}