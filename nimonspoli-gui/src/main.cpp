#include "../lib/raylib/include/raylib.h"
#include "views/GUIManager.hpp"
#include "views/screens/GameScreen.hpp"
#include "views/screens/MainMenuScreen.hpp"
#include "views/screens/WinScreen.hpp"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1920, 1080, "Nimonspoli");
    // ToggleFullscreen();
    SetTargetFPS(60);

    std::string title = "Nimonspoli";
    GUIManager gui(1920, 1080, title, 60);

    MainMenuScreen* menuScreen = new MainMenuScreen();
    GameScreen* gameScreen = new GameScreen();
    WinScreen* winScreen = new WinScreen();

    gui.setScreen(menuScreen);

    while(!WindowShouldClose()){
        float dt = GetFrameTime();

        if (menuScreen->isReadyToStart()){
            gameScreen->setPlayerCount(menuScreen->getSetup().playerCount);
            //TODO: gameScreen->applySetup(menuScreen->getSetup().names);
            menuScreen->resetReady();
            gui.setScreen(gameScreen);
        }

        if (gameScreen->isGameOver()) {
            // Mock results untuk testing
            std::vector<PlayerResult> results = {
                {"Uname1", 5000, 6, 2, false, 1, true,  {220,50,50,255}},
                {"Uname2", 3000, 3, 1, false, 2, false, {240,200,50,255}},
                {"Uname3", 1500, 2, 0, false, 3, false, {50,180,50,255}},
                {"Uname4",    0, 0, 0, true,  -1,false, {50,200,220,255}},
            };
            winScreen->setResults(results, WinScenario::MAX_TURN);
            gameScreen->gameOver = false; // reset
            gui.setScreen(winScreen);
        }

        if (winScreen->goToMainMenu()){
            winScreen->reset();
            gui.setScreen(menuScreen);
        }
        if(winScreen->goToExit()) break;

        // Update + Render;
        BeginDrawing();
        if (gui.getCurrentScreen()){
            gui.getCurrentScreen()->update(dt);
            gui.getCurrentScreen()->render(gui.getWindow());
            EndDrawing();
        }
    }
    delete menuScreen;
    delete gameScreen;
    delete winScreen;
    CloseWindow();
    return 0;
}
