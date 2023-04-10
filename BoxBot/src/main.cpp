#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>


struct Shape {
    int x, y, width, height;
    uint32_t color;
};

bool fits(int x, int y, int width, int height, const std::vector<SDL_Rect>& rects) {
    SDL_Rect rect = { x, y, width, height };
    for (const auto& r : rects) {
        if (SDL_HasIntersection(&rect, &r)) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* args[]) {
    std::vector<Shape> shapes = {
        { 0, 0, 100, 100, 0xFFFF0000 },   // type 1 (red)
        { 0, 0, 200, 100, 0xFF00FF00 },   // type 2 (green)
        { 0, 0, 100, 200, 0xFF0000FF },   // type 3 (blue)
        { 0, 0, 50, 100, 0xFFFFFF00 },    // type 4 (yellow)
        { 0, 0, 100, 50, 0xFF00FFFF },    // type 5 (cyan)
        { 0, 0, 150, 50, 0xFFFF00FF },    // type 6 (magenta)
        { 0, 0, 50, 150, 0xFF808080 },    // type 7 (gray)
        { 0, 0, 100, 100, 0xFFFFFFFF },   // type 8 (white)
        { 0, 0, 50, 50, 0xFFFF8000 },     // type 9 (orange)
        { 0, 0, 100, 150, 0xFF800080 },   // type 10 (purple)
        { 0, 0, 150, 100, 0xFF008080 }    // type 11 (teal)
    };

    std::sort(shapes.begin(), shapes.end(),
        [](const Shape& a, const Shape& b) { return a.width * a.height > b.width * b.height; });

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Packing Shapes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 800, 600);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    std::vector<SDL_Rect> rects;
    for (const auto& shape : shapes) {
        bool found = false;
        for (int y = 0; y < 600 && !found; y++) {
            for (int x = 0; x < 800 && !found; x++) {
                if (fits(x, y, shape.width, shape.height, rects)) {
                    SDL_Rect rect = { x, y, shape.width, shape.height };
                    SDL_SetRenderDrawColor(renderer, (shape.color >> 16) & 0xFF, (shape.color >> 8) & 0xFF, shape.color & 0xFF, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    rects.push_back(rect);
                    found = true;
                }
            }
        }
    }
    SDL_RenderPresent(renderer);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::ofstream file;
    file.open("output.gcode");

    for (const auto& rect : rects) {
        file << "G1 X" << rect.x << " Y" << rect.y << std::endl;
        file << "G1 X" << (rect.x + rect.w) << " Y" << rect.y << std::endl;
        file << "G1 X" << (rect.x + rect.w) << " Y" << (rect.y + rect.h) << std::endl;
        file << "G1 X" << rect.x << " Y" << (rect.y + rect.h) << std::endl;
        file << "G1 X" << rect.x << " Y" << rect.y << std::endl;
    }

    file.close();


    return 0;
}