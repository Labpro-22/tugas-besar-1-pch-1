#include "GUIManager.hpp"
#include "screens/GameScreen.hpp"

GUIManager::GUIManager(int w, int h, string& t, int fps) : window(w, h, t, fps) {}

GUIManager::~GUIManager() {
    shutdown();
}

void GUIManager::setScreen(IScreen* screen) {
    if (currentScreen) currentScreen->onExit();
    currentScreen = screen;
    if (currentScreen) currentScreen->onEnter();
}

void GUIManager::pushCommand(Command* cmd) {
    if (cmd) commandQueue.push(cmd);
}

void GUIManager::flushCommands() {
    while (!commandQueue.empty()) {
        Command* cmd = commandQueue.front();
        commandQueue.pop();
        if (cmd && gameMaster) {
            cmd->execute(*gameMaster);
        }
        delete cmd;
    }
}

void GUIManager::shutdown() {
    if (currentScreen) {
        currentScreen->onExit();
        currentScreen = nullptr;
    }
    // Bersihkan sisa command yang belum dieksekusi
    while (!commandQueue.empty()) {
        delete commandQueue.front();
        commandQueue.pop();
    }
}
