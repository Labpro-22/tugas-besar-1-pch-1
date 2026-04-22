#pragma once
#include "../IScreen.hpp"
#include "../Window.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "GameResult.hpp"


#include <string>
#include <vector>


// ─── WinScreen ───────────────────────────────────────────────────────────────
class WinScreen : public IScreen {
public:
    WinScreen();
    ~WinScreen() override;

    void onEnter() override;
    void onExit()  override;
    void update(float dt) override;
    void render(Window& window) override;

    // Isi data sebelum setScreen() dipanggil
    void setResults(const std::vector<PlayerResult>& results, WinScenario scenario);

    // GUIManager cek ini untuk tahu harus pindah ke mana
    bool goToMainMenu() const { return wantsMainMenu; }
    bool goToExit()     const { return wantsExit; }
    void reset(){wantsMainMenu = false; wantsExit = false;}
private:
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;

    std::vector<PlayerResult> results;  // sudah diurutkan dari rank 1
    WinScenario scenario;
    float glowTimer;

    // Navigasi
    bool wantsMainMenu;
    bool wantsExit;

    void drawHeader();
    void drawRankingMaxTurn();   // tampilan ranking lengkap
    void drawRankingBankruptcy();// tampilan 1 pemenang saja
    void drawButtons();

    // Sort results berdasarkan spec: uang → properti → kartu
    void sortAndRankResults();
};