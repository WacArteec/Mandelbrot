#include <TXLib.h>
#include <math.h>
#include <Windows.h>

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

int Mandelbrot(double creal, double cimage)
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

void DrawMandelbrot(Coords *coords)
{
    DWORD lastTime = GetTickCount();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            double real = coords->minX + (coords->maxX - coords->minX) * x / WIDTH;
            double imag = coords->minY + (coords->maxY - coords->minY) * y / HEIGHT;

            int iter = Mandelbrot(real, imag);

            COLORREF color = (iter == MAX_ITER) ? TX_BLACK : RGB((iter * 1) % 255, (iter * 3) % 255, (iter * 2) % 255);

            txSetPixel(x, y, color);
        }
    }

    DWORD currentTime = GetTickCount();

    float fps = 1.0 / (currentTime - lastTime);

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
            CompareFloats(coords.minY, old_coords.minY) )
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