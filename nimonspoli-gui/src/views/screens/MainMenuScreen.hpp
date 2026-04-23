#pragma once
#include "../IScreen.hpp"
#include "../Window.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../../src/core/ComputerPlayer/ComputerPlayer.hpp"
#include <string>
#include <vector>
#include <array>

// ─── Data yang dikumpulkan dari main menu ────────────────────────────────────
struct GameSetup {
    int playerCount;                    // 2-4
    std::array<std::string, 4> names;   // username tiap slot
    std::array<bool, 4>        isBot;   // true = COM
    COMDifficulty              botDifficulty; // difficulty global untuk semua COM
    std::string saveFile;               // kosong = new game, isi = load game
    bool isLoadGame;
};

// ─── MainMenuScreen ──────────────────────────────────────────────────────────
class MainMenuScreen : public IScreen {
public:
    MainMenuScreen();
    ~MainMenuScreen() override;

    void onEnter() override;
    void onExit()  override;
    void update(float dt) override;
    void render(Window& window) override;

    bool isReadyToStart() const { return readyToStart; }
    const GameSetup& getSetup() const { return setup; }
    void resetReady(){ readyToStart = false; }

private:
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;

    Color playerColors[4] = {
        {220,  50,  50, 255},
        {240, 200,  50, 255},
        { 50, 180,  50, 255},
        { 50, 200, 220, 255},
    };

    // ── State UI ──────────────────────────────────────────────────────────
    int   selectedPlayerCount;
    int   focusedInput;
    float glowTimer;

    std::string nameBuffers[4];

    // ── COM state ─────────────────────────────────────────────────────────
    bool          isBot[4];             // toggle per slot
    COMDifficulty selectedDifficulty;   // global untuk semua COM

    // ── Save file state ───────────────────────────────────────────────────
    bool                     saveFileExists;
    std::vector<std::string> saveFiles;
    int                      selectedSaveIdx;

    // ── Load panel state ──────────────────────────────────────────────────
    bool   showLoadPanel;
    double lastClickTime;

    // ── Output state ──────────────────────────────────────────────────────
    bool      readyToStart;
    GameSetup setup;

    // ── Error state ───────────────────────────────────────────────────────
    std::string errorMsg;
    float       errorTimer;

    // ── Private methods ───────────────────────────────────────────────────
    void handleInput();
    void drawTitle();
    void drawLeftPanel();
    void drawRightPanel();
    void drawLoadPanel();
    void drawError();

    bool validateInputs();
    void scanSaveFiles();
    void triggerLoad(const std::string& path);
    void applySetup();
};