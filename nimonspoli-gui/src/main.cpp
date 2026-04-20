#include "../lib/raylib/include/raylib.h"
#include "views/GUIManager.hpp"
#include "views/screens/GameScreen.hpp"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1920, 1080, "Nimonspoli");
    // ToggleFullscreen();
    SetTargetFPS(60);

    std::string title = "Nimonspoli";
    GUIManager gui(1920, 1080, title, 60);

    GameScreen gameScreen;
    gui.setScreen(&gameScreen);
    gui.run();

    CloseWindow();
    return 0;
}