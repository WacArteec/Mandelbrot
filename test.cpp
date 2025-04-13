//g++ -mavx2 -O3 test.cpp -o test && ./test
//g++ -mavx2 test.cpp -o test && ./test
#include <stdio.h>
#include <immintrin.h>
#include <chrono>

#define WIDTH 800
#define HEIGHT 700
#define MAX_ITER 60

struct Coords
{
    double minX;
    double maxX;
    double minY;
    double maxY;
};

int MandelbrotBase(double creal, double cimage)
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

void CountMandelbrot(double *real, double *imag, int* iters)
{
    for (int i = 0; i < 4; i++)
        iters[i] = MandelbrotBase(real[i], imag[i]);

}

void Mandelbrot_AVX(double *real, double *imag, int* iters)
{
    __m256d creal = _mm256_loadu_pd(real);
    __m256d cimage = _mm256_loadu_pd(imag);

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

        __m256i add_iters = _mm256_and_si256(_mm256_castpd_si256(mask), arr_of_one); // у всех точек, что не выбыли, добавляем единицу к количеству циклов
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

double testMandlrot(Coords *coords, void (*Mandelbrot)(double *, double *, int *))
{
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x += 4)
        {
            double real[4] __attribute__((aligned(64)));
            double imag[4] __attribute__((aligned(64)));

            int iters[4] __attribute__((aligned(32)));

            for (size_t i = 0; i < 4; i++)
            {
                real[i] = coords->minX + (coords->maxX - coords->minX) * (x + i) / WIDTH;
                imag[i] = coords->minY + (coords->maxY - coords->minY) * y / HEIGHT;
            }

            for(int i = 0; i < 100; i++)
                Mandelbrot(real, imag, iters);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();
}

int main()
{
    Coords coords = {-2.0, 1.0, -1.5, 1.5};

    printf("blank array %g\n", testMandlrot(&coords, CountMandelbrot));
    printf("blank AVX %g \n", testMandlrot(&coords, Mandelbrot_AVX));

    double time_array = 0.0, time_AVX = 0.0;

    for (size_t i = 0; i < 50; i++)
    time_array += testMandlrot(&coords, CountMandelbrot);

    
    printf("array time test1 %g\n", time_array);


    for (size_t i = 0; i < 50; i++)
    time_AVX += testMandlrot(&coords, Mandelbrot_AVX);
    
    printf("AVX time test1 %g \n", time_AVX);

    printf("average: \narray: %g \nAVX: %g", time_array/50, time_AVX/50);

    return 0;
}