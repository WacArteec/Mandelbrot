#include "Graphics.h"

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

    Graphic(Coords *coords_, sf::RenderWindow &window_, sf::Image &image_, sf::Texture &texture_, sf::Sprite &sprite_)
        : coords(coords_), window(window_), image(image_), texture(texture_), sprite(sprite_) {}
};

int MandelbrotBase(double creal, double cimage);
void DrawMandelbrot(Graphic *graphic, void (*Mandelbrot)(double *, double *, int *));
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

double DrawFrame(Graphic *graphic, void (*Mandelbrot)(double *, double *, int *))
{
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x += 4)
        {
            double real[4] = {0};

            double imag[4] = {0};

            int iters[4] __attribute__((aligned(32)));

            for (size_t i = 0; i < 4; i++)
            {
                real[i] = graphic->coords->minX + (graphic->coords->maxX - graphic->coords->minX) * (x + i) / WIDTH;
                imag[i] = graphic->coords->minY + (graphic->coords->maxY - graphic->coords->minY) * y / HEIGHT;
            }

            Mandelbrot(real, imag, iters);

            for (int i = 0; i < 4; i++)
                graphic->image.setPixel(x + i, y, GetColor(iters[i]));
        }
    }

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

void DrawMandelbrot(Counters *counters)
{
    unsigned index = 0;

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            coords.minX -= moveStep;
            coords.maxX -= moveStep;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            coords.minX += moveStep;
            coords.maxX += moveStep;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            coords.minY += moveStep;
            coords.maxY += moveStep;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            coords.minY -= moveStep;
            coords.maxY -= moveStep;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            moveStep *= 1.1;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        {
            moveStep *= 0.9;

            redraw = true;
        }

        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
        {
            index = (index + 1) % 3;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        {
            double centerX = (coords.minX + coords.maxX) / 2;
            double centerY = (coords.minY + coords.maxY) / 2;
            double newWidth = (coords.maxX - coords.minX) / zoomFactor;
            double newHeight = (coords.maxY - coords.minY) / zoomFactor;

            coords.minX = centerX - newWidth / 2;
            coords.maxX = centerX + newWidth / 2;
            coords.minY = centerY - newHeight / 2;
            coords.maxY = centerY + newHeight / 2;

            redraw = true;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        {
            double centerX = (coords.minX + coords.maxX) / 2;
            double centerY = (coords.minY + coords.maxY) / 2;
            double newWidth = (coords.maxX - coords.minX) * zoomFactor;
            double newHeight = (coords.maxY - coords.minY) * zoomFactor;

            coords.minX = centerX - newWidth / 2;
            coords.maxX = centerX + newWidth / 2;
            coords.minY = centerY - newHeight / 2;
            coords.maxY = centerY + newHeight / 2;

            redraw = true;
        }

        if (redraw | !redraw) // заменить на if(redraw) после тестов и показа ментору
        {
            time += DrawFrame(&graphic, counters[index]);
            old_coords = coords;

            draws++;

            if (draws % 1000 == 0)
            {
                printf("%s average time: %g\n", funcs[index], time / 1000);
                time = 0.0;

                index = (index + 1) % 3;

                if (draws == 3000)
                    return;
            }
        }
    }
}