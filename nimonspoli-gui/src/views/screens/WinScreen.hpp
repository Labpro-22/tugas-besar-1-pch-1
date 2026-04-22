#pragma once
#include "../IScreen.hpp"
#include "../Window.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include <string>
#include <vector>

// ─── Data satu pemain untuk rekap ────────────────────────────────────────────
// Diisi dari GameState saat game selesai
struct PlayerResult {
    std::string username;
    int         money;          // uang akhir
    int         propertyCount;  // jumlah properti dimiliki
    int         cardCount;      // jumlah kartu di tangan
    bool        bankrupt;       // apakah sudah bangkrut
    int         rank;           // 1 = pemenang, dst. Bisa lebih dari 1 yang rank 1 kalau seri
    bool        isWinner;       // true kalau rank 1
    Color       color;          // warna player
};

// ─── Skenario akhir game ──────────────────────────────────────────────────────
enum class WinScenario {
    MAX_TURN,   // batas giliran habis → ranking lengkap
    BANKRUPTCY  // hanya 1 tersisa → 1 pemenang
};

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