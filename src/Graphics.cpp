#include <SFML/Graphics.hpp>

#include "../include/Graphics.h"

struct Coords
{
    double minX;
    double maxX;
    double minY;
    double maxY;
};

struct Graphic
{
    Coords *coords;
    sf::RenderWindow &window;
    sf::Image &image;
    sf::Texture &texture;
    sf::Sprite &sprite;
};

typedef void (*FrameCounter)(Graphic *);

sf::Color GetColor(int iter);

sf::Color GetColor(int iter)
{
    if (iter == MAX_ITER)
        return sf::Color::Black;

    int r = (iter * 1) % 255;
    int g = (iter * 3) % 255;
    int b = (iter * 2) % 255;

    return sf::Color(r, g, b);
}

void NaiveCountFrame(Graphic *graphic)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x ++)
        {
            double real;

            double imag;

            int iters;

                real = graphic->coords->minX + (graphic->coords->maxX - graphic->coords->minX) * x / WIDTH;
                imag = graphic->coords->minY + (graphic->coords->maxY - graphic->coords->minY) * y / HEIGHT;

            NaiveMandelbrot(real, imag, &iters);

            graphic->image.setPixel(x, y, GetColor(iters));
        }
    }
}

void ArrayCountFrame(Graphic *graphic)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x += 4)
        {
            double real[4];

            double imag[4];

            int iters[4];

            for (size_t i = 0; i < 4; i++)
            {
                real[i] = graphic->coords->minX + (graphic->coords->maxX - graphic->coords->minX) * (x + i) / WIDTH;
                imag[i] = graphic->coords->minY + (graphic->coords->maxY - graphic->coords->minY) * y / HEIGHT;
            }

            ArrayMandelbrot(real, imag, iters);

            for (int i = 0; i < 4; i++)
                graphic->image.setPixel(x + i, y, GetColor(iters[i]));
        }
    }
}

void AVXCountFrame(Graphic *graphic)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x += 4)
        {
            alignas(32) double real[4];

            alignas(32) double imag[4];

            alignas(32) int iters[4];

            for (size_t i = 0; i < 4; i++)
            {
                real[i] = graphic->coords->minX + (graphic->coords->maxX - graphic->coords->minX) * (x + i) / WIDTH;
                imag[i] = graphic->coords->minY + (graphic->coords->maxY - graphic->coords->minY) * y / HEIGHT;
            }

            AVXMandelbrot(real, imag, iters);

            for (int i = 0; i < 4; i++)
            {
                //printf("%g %g %d\n", real[i], imag[i], iters[i]);
                graphic->image.setPixel(x + i, y, GetColor(iters[i]));
            }
        }
    }
}

double DrawFrame(Graphic *graphic, void (*FrameCounter)(Graphic *))
{
    auto start = std::chrono::high_resolution_clock::now();

    FrameCounter(graphic);

    graphic->texture.loadFromImage(graphic->image);
    graphic->sprite.setTexture(graphic->texture);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    double fps = 1.0 / elapsed.count();

    graphic->window.clear();
    graphic->window.draw(graphic->sprite);

    sf::Font font;

    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    sf::Text text;

    text.setFont(font);
    text.setString("FPS: " + std::to_string(fps));
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 10);

    graphic->window.draw(text);

    graphic->window.display();

    return elapsed.count();
}

void DrawMandelbrot()
{
    unsigned index = 0;

    FrameCounter counters[3] = {AVXCountFrame, NaiveCountFrame, ArrayCountFrame};

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    window.setFramerateLimit(60);

    sf::Image image;
    image.create(WIDTH, HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    Coords coords = {-2.0, 1.0, -1.5, 1.5};
    Coords old_coords = coords;

    Graphic graphic = {&coords, window, image, texture, sprite};

    DrawFrame(&graphic, counters[index]);

    double moveStep = 0.1;
    double zoomFactor = 1.5;

    unsigned draws = 0; // только для тестов
    double time = 0.0;  //
    char funcs[3][6];   //

    strcpy(funcs[0], "Naive"); //
    strcpy(funcs[1], "Array"); //
    strcpy(funcs[2], "AVX  "); //

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        bool redraw = false;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
        {
            coords.minX -= moveStep;
            coords.maxX -= moveStep;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
        {
            coords.minX += moveStep;
            coords.maxX += moveStep;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
        {
            coords.minY += moveStep;
            coords.maxY += moveStep;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
        {
            coords.minY -= moveStep;
            coords.maxY -= moveStep;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
        {
            moveStep *= 1.1;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
        {
            moveStep *= 0.9;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
        {
            index = (index + 1) % 3;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O)
        {
            double centerX = (coords.minX + coords.maxX) / 2;
            double centerY = (coords.minY + coords.maxY) / 2;
            double newWidth = (coords.maxX - coords.minX) / zoomFactor;
            double newHeight = (coords.maxY - coords.minY) / zoomFactor;

            coords.minX = centerX - newWidth / 2;
            coords.maxX = centerX + newWidth / 2;
            coords.minY = centerY - newHeight / 2;
            coords.maxY = centerY + newHeight / 2;

            moveStep /= zoomFactor;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
        {
            double centerX = (coords.minX + coords.maxX) / 2;
            double centerY = (coords.minY + coords.maxY) / 2;
            double newWidth = (coords.maxX - coords.minX) * zoomFactor;
            double newHeight = (coords.maxY - coords.minY) * zoomFactor;

            coords.minX = centerX - newWidth / 2;
            coords.maxX = centerX + newWidth / 2;
            coords.minY = centerY - newHeight / 2;
            coords.maxY = centerY + newHeight / 2;

            moveStep *= zoomFactor;

            redraw = true;
        }

        if (redraw /*| !redraw*/) // заменить на if(redraw) после тестов и показа ментору
        {
            time += DrawFrame(&graphic, counters[index]);
            old_coords = coords;

            draws++;

            /*if (draws % 1000 == 0)
            {
                printf("%s average time: %g\n", funcs[index], time / 1000);
                time = 0.0;

                index = (index + 1) % 3;

                if (draws == 3000)
                    return;
            }*/
        }
    }
}