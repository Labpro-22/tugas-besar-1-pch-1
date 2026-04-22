#pragma once
#include <string>
#include "Window.hpp"
#include "IScreen.hpp"
using namespace std;

class GUIManager{
    private:
        Window window;
        IScreen* currentScreen = nullptr;
        void flushCommands();
        // TODO: Commands, queue<Command*> commandQueue;
        // TODO: GameMaster& gameMaster  -> referensi ke core game untuk eksekusi command
    public:
        GUIManager(int w, int h, string& t, int fps);
        ~GUIManager();
        void setScreen(IScreen* screen);
        void run();
        void pushCommand();
       void shutdown();
       IScreen* getCurrentScreen() const { return currentScreen; }
       Window& getWindow(){return window;}
};
