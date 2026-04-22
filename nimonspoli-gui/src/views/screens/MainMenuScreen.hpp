#pragma once
#include "../IScreen.hpp"
#include "../Window.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include <string>
#include <vector>
#include <array>

// ─── Data yang dikumpulkan dari main menu ────────────────────────────────────
// Ini yang akan diteruskan ke GameScreen saat game dimulai
struct GameSetup {
    int playerCount;                    // 2-4
    std::array<std::string, 4> names;   // username tiap slot
    std::string saveFile;               // kosong = new game, isi = load game
    bool isLoadGame;
};

// ─── MainMenuScreen ──────────────────────────────────────────────────────────
// Inherit IScreen seperti GameScreen — konsisten dengan arsitektur GUI
class MainMenuScreen : public IScreen {
public:
    MainMenuScreen();
    ~MainMenuScreen() override;

    void onEnter() override;
    void onExit()  override;
    void update(float dt) override;
    void render(Window& window) override;

    // GUIManager bisa cek ini setiap frame —
    // kalau true, berarti user sudah klik MULAI / LOAD dan siap pindah ke GameScreen
    bool isReadyToStart() const { return readyToStart; }
    const GameSetup& getSetup() const { return setup; }
    void resetReady(){readyToStart = false;}
private:
    // ── Warna tema (sama dengan GameScreen) ──────────────────────────────
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;

    // Warna per player — harus sama persis dengan GameScreen::playerColors
    Color playerColors[4] = {
        {220,  50,  50, 255},   // P1 merah
        {240, 200,  50, 255},   // P2 kuning
        { 50, 180,  50, 255},   // P3 hijau
        { 50, 200, 220, 255},   // P4 cyan
    };

    // ── State UI ──────────────────────────────────────────────────────────
    int   selectedPlayerCount;  // berapa pemain yang dipilih (2/3/4)
    int   focusedInput;         // input mana yang sedang aktif (-1 = tidak ada)
    float glowTimer;            // untuk efek pulse pada border aktif

    // Buffer teks untuk tiap input username (max 4 pemain)
    // Diketik realtime dengan GetCharPressed()
    std::string nameBuffers[4];

    // Save file detection
    bool  saveFileExists;
    std::string saveFileName;

    // Output state
    bool      readyToStart;
    GameSetup setup;

    // ── Error state ───────────────────────────────────────────────────────
    std::string errorMsg;       // pesan error validasi
    float       errorTimer;     // berapa lama error ditampilkan

    // ── Private methods ───────────────────────────────────────────────────
    void handleInput();
    void drawTitle();
    void drawPlayerCountSelector();
    void drawUsernameInputs();
    void drawRightPanel();
    void drawError();
    void drawLeftPanel();

    bool validateInputs();      // cek username tidak kosong/duplikat
    void checkSaveFile();       // cek apakah ada save file
    void applySetup();          // kumpulkan data ke struct GameSetup
};