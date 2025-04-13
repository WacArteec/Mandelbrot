#include "Array/Array.h"
#include "AVX/AVX.h"
#include "Naive/Naive.h"
#include "Graphics/Graphics.h"
#include "enum.h"

int main()
{
    Counters counters[3] = {Naive, Array, AVX};

    for (size_t i = 0; i < 7; i++)
    {
        printf("%I64lu \n", i);

        DrawMandelbrot(counters);
    }
    

    return 0;
}