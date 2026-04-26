#pragma once
#include "../../lib/raylib/include/raylib.h"
#include <string>
using namespace std;

class Window{
    private:
        int width, height, targetFPS;
        string title;
    public:
        Window(int width, int height, string& title, int targetFPS = 60);
        ~Window();

        bool isOpen() const;
        void beginFrame();
        void endFrame();
        int getHeigth() const;
        int getWidth() const;
        int getFPS() const;
        const string& getTitle() const;
};
