#pragma once
#include <string>
#include <queue>
#include "Window.hpp"
#include "IScreen.hpp"
#include "../core/Commands/Command.hpp"
#include "../core/GameMaster/GameMaster.hpp"
using namespace std;

// Forward declaration — hindari circular include
class GameMaster;

class GUIManager {
private:
    Window window;
    IScreen* currentScreen = nullptr;
    GameMaster* gameMaster = nullptr;        // opsional — di-set via setGameMaster()
    queue<Command*> commandQueue;
    

public:
    GUIManager(int w, int h, string& t, int fps);
    ~GUIManager();

    void setScreen(IScreen* screen);
    void setGameMaster(GameMaster* gm) { gameMaster = gm; }
    GameMaster* getGameMaster() const  { return gameMaster; }

    // Antri command untuk dieksekusi di awal frame berikutnya
    void pushCommand(Command* cmd);

    void shutdown();
    IScreen* getCurrentScreen() const { return currentScreen; }
    Window&  getWindow()              { return window; }

    void flushCommands();
    void clearCommands() {
    while (!commandQueue.empty()) {
        delete commandQueue.front();
        commandQueue.pop();
    }
}
};