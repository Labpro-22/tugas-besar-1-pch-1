#include "Window.hpp"

Window::Window(int w, int h, string &t, int fps)
    : height(720), width(1280), title(t), targetFPS(fps)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, t.c_str());
    SetTargetFPS(fps);
}

Window::~Window()
{
    if (IsWindowReady())
        CloseWindow();
}

bool Window::isOpen() const { return !WindowShouldClose(); }
void Window::beginFrame()
{
    BeginDrawing();
    ClearBackground({245, 240, 232, 255});
} // Warna background}
void Window::endFrame() { EndDrawing(); }
int Window::getWidth() const { return width; }
int Window::getHeigth() const { return height; }
const string &Window::getTitle() const { return title; }
int Window::getFPS() const { return targetFPS; }
