#include <TXLib.h>
// #include <math.h>
#include <Windows.h>
#include <immintrin.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITER 60

struct Coords
{
    double minX;
    double maxX;
    double minY;
    double maxY;
};

int CompareFloats(double num1, double num2);
int Mandelbrot(double creal, double cimage);
void DrawMandelbrot(Coords *coords);

int CompareFloats(double num1, double num2)
{
    int ret = ((num1 - num2) < 1e-10) ? 0 : 1;
    return ret;
}

__m256i Mandelbrot_AVX(__m256d creal, __m256d cimage)
{
    __m256d zreal = _mm256_setzero_pd();  // double zreal = 0.0, zimage = 0.0;
    __m256d zimage = _mm256_setzero_pd(); //
    __m256i iter = _mm256_setzero_si256();

    __m256d arr_of_ten = _mm256_set1_pd(10.0);
    __m256d arr_of_one = _mm256_set1_epi64x(1);
    __m256d arr_of_two = _mm256_set1_pd(2.0);

    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        __m256d zr2 = _mm256_mul_pd(zreal, zreal);   // double zr2 = zreal * zreal;
        __m256d zi2 = _mm256_mul_pd(zimage, zimage); // double zi2 = zimage * zimage;

        __m256d mask = _mm256_cmp_pd(_mm256_add_pd(zr2, zi2),
                                     arr_of_ten,
                                     _CMP_LE_OQ); // if (zr2 + zi2 <= 10.0) _CMP_LE_OQ - это <=

        __m256i add_iters = _mm256_and_si256(_mm256_castpd_si256(mask), arr_of_one); // у всех точек, что не выбыли, добавляем единицу к количеству циклов
        iter = _mm256_add_epi64(iter, add_iters);                                    //

        if (_mm256_testz_pd(mask) == 1) // возвращаетя 1, если все нули
            break;

        __m256d new_zr = _mm256_add_pd(_mm256_sub_pd(zr2, zi2) + creal);                            // double new_zr = zr2 - zi2 + creal;
        __m256d new_zi = _mm256_add_pd(_mm256_mul_pd(arr_of_two, _mm256_mul_pd(zr2, zi2)) + creal); // double new_zi = 2 * zreal * zimage + cimage;

        zreal = _mm256_blendv_pd(zreal, new_zr, mask);   // zreal = new_zr;
        zimage = _mm256_blendv_pd(zimage, new_zi, mask); // zimage = new_zi;
    }

    return iter;
}

void DrawMandelbrot(Coords *coords)
{
    DWORD lastTime = GetTickCount();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x += 4)
        {
            double real[4] = {0};
            double imag[4] = {0};

            for (size_t i = 0; i < 4; i++)
            {
                real[i] = coords->minX + (coords->maxX - coords->minX) * (x + i) / WIDTH;
                imag[i] = coords->minY + (coords->maxY - coords->minY) * y / HEIGHT;
            }

            __m256d real_fast = _mm256_loadu_pd(real);
            __m256d image_fast = _mm256_loadu_pd(imag);

            __m256i iterations = Mandelbrot(real_fast, image_fast);

            int iter[4] = {0};

            for (size_t i = 0; i < 4; i++)
            {
                COLORREF color = (iter[i] == MAX_ITER) ? TX_BLACK : RGB((iter * 1) % 255, (iter * 3) % 255, (iter * 2) % 255);

                txSetPixel(x, y, color);
            }
        }
    }

    DWORD currentTime = GetTickCount();

    float fps = 1000 / (currentTime - lastTime);

    char buffer[32];
    sprintf(buffer, "FPS: %g", fps);
    txTextOut(10, 10, buffer);
}

int main()
{
    txCreateWindow(WIDTH, HEIGHT);
    txSetColor(TX_RED);

    Coords coords = {-1.5, 1.5, -1.5, 1.5};
    Coords old_coords = coords;

    double moveStep = 1;
    double zoomFactor = 1.5;

    DrawMandelbrot(&coords);
    txSleep();

    while (!txGetAsyncKeyState(VK_ESCAPE))
    {
        if (txGetAsyncKeyState(VK_RIGHT))
        {
            coords.minX -= moveStep;
            coords.maxX -= moveStep;
        }

        else if (txGetAsyncKeyState(VK_LEFT))
        {
            coords.minX += moveStep;
            coords.maxX += moveStep;
        }

        else if (txGetAsyncKeyState(VK_UP))
        {
            coords.minY += moveStep;
            coords.maxY += moveStep;
        }

        else if (txGetAsyncKeyState(VK_DOWN))
        {
            coords.minY -= moveStep;
            coords.maxY -= moveStep;
        }

        else if (txGetAsyncKeyState('A'))
            moveStep *= 1.1;

        else if (txGetAsyncKeyState('Z'))
            moveStep *= 0.9;

        else if (txGetAsyncKeyState(VK_ADD))
        {
            double centerX = (coords.minX + coords.maxX) / 2;
            double centerY = (coords.minY + coords.maxY) / 2;
            double newWidth = (coords.maxX - coords.minX) / zoomFactor;
            double newHeight = (coords.maxY - coords.minY) / zoomFactor;

            coords.minX = centerX - newWidth / 2;
            coords.maxX = centerX + newWidth / 2;
            coords.minY = centerY - newHeight / 2;
            coords.maxY = centerY + newHeight / 2;
        }

        else if (txGetAsyncKeyState(VK_SUBTRACT))
        {
            double centerX = (coords.minX + coords.maxX) / 2;
            double centerY = (coords.minY + coords.maxY) / 2;
            double newWidth = (coords.maxX - coords.minX) * zoomFactor;
            double newHeight = (coords.maxY - coords.minY) * zoomFactor;

            coords.minX = centerX - newWidth / 2;
            coords.maxX = centerX + newWidth / 2;
            coords.minY = centerY - newHeight / 2;
            coords.maxY = centerY + newHeight / 2;
        }

        if (CompareFloats(coords.maxX, old_coords.maxX) ||
            CompareFloats(coords.maxY, old_coords.maxY) ||
            CompareFloats(coords.minX, old_coords.minX) ||
            CompareFloats(coords.minY, old_coords.minY))
        {
            DrawMandelbrot(&coords);
            txSleep();

            old_coords.maxX = coords.maxX;
            old_coords.maxY = coords.maxY;
            old_coords.minX = coords.minX;
            old_coords.minY = coords.minY;
        }
    }

    return 0;
}