#pragma once
#include "Window.hpp"

class IScreen{
    public:
        virtual ~IScreen() {};
        virtual void onEnter() {}
        virtual void onExit(){}
        virtual void update(float dt = 0) {};
        virtual void render(Window& window) {};
};