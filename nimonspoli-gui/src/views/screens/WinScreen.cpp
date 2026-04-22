#include "WinScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include <algorithm>
#include <cmath>

WinScreen::WinScreen()
    : scenario(WinScenario::MAX_TURN), glowTimer(0.f),
      wantsMainMenu(false), wantsExit(false) {}

WinScreen::~WinScreen() {}

// ─── setResults ───────────────────────────────────────────────────────────────
// Dipanggil dari luar (GUIManager/GameScreen) sebelum screen ini diaktifkan
// Isi dengan data real dari GameState saat backend sudah siap
void WinScreen::setResults(const std::vector<PlayerResult>& r, WinScenario sc) {
    results  = r;
    scenario = sc;
    sortAndRankResults();
}

// ─── sortAndRankResults ───────────────────────────────────────────────────────
// Implementasi tiebreaker sesuai spec:
// 1. Uang terbanyak
// 2. Jumlah properti terbanyak (kalau uang seri)
// 3. Jumlah kartu terbanyak (kalau properti seri)
// 4. Kalau masih seri → semua yang seri dapat rank sama (isWinner = true)
void WinScreen::sortAndRankResults() {
    // Sort: bankrupt ke bawah dulu, lalu tiebreaker
    std::sort(results.begin(), results.end(), [](const PlayerResult& a, const PlayerResult& b) {
        if (a.bankrupt != b.bankrupt) return !a.bankrupt; // non-bankrupt duluan
        if (a.money != b.money)           return a.money > b.money;
        if (a.propertyCount != b.propertyCount) return a.propertyCount > b.propertyCount;
        return a.cardCount > b.cardCount;
    });

    // Assign rank
    // Pemain yang nilainya sama persis → rank sama
    int currentRank = 1;
    for (int i = 0; i < (int)results.size(); i++) {
        if (results[i].bankrupt) {
            results[i].rank     = -1;   // bankrupt tidak dapat rank
            results[i].isWinner = false;
            continue;
        }
        if (i == 0) {
            results[i].rank = 1;
        } else {
            // Cek apakah sama persis dengan sebelumnya
            auto& prev = results[i-1];
            bool seri  = (!prev.bankrupt &&
                          results[i].money == prev.money &&
                          results[i].propertyCount == prev.propertyCount &&
                          results[i].cardCount == prev.cardCount);
            results[i].rank = seri ? prev.rank : currentRank;
        }
        currentRank = results[i].rank + 1;
        results[i].isWinner = (results[i].rank == 1);
    }
}

void WinScreen::onEnter() {
    wantsMainMenu = false;
    wantsExit     = false;
    glowTimer     = 0.f;

    // Mock data — hapus ini saat backend sudah siap
    // dan ganti dengan setResults() yang dipanggil dari luar
    if (results.empty()) {
        results = {
            {"Cici",  5000, 6, 2, false, 1, true,  { 50,180, 50,255}},
            {"Budi",  5000, 5, 4, false, 2, false, {240,200, 50,255}},
            {"Andra", 4500, 3, 1, false, 3, false, {220, 50, 50,255}},
            {"Dodi",     0, 0, 0, true,  -1,false, { 50,200,220,255}},
        };
        scenario = WinScenario::MAX_TURN;
        sortAndRankResults();
    }
}

void WinScreen::onExit() { results.clear(); }

void WinScreen::update(float dt) {
    glowTimer += dt;
}

void WinScreen::render(Window& window) {
    (void)window;
    ClearBackground({14, 15, 31, 255});
    drawHeader();

    if (scenario == WinScenario::MAX_TURN)
        drawRankingMaxTurn();
    else
        drawRankingBankruptcy();

    drawButtons();
}

// ─── drawHeader ───────────────────────────────────────────────────────────────
void WinScreen::drawHeader() {
    const char* sub  = (scenario == WinScenario::MAX_TURN)
                       ? "BATAS GILIRAN TERCAPAI"
                       : "SEMUA PEMAIN LAIN BANGKRUT";

    // Label kecil di atas
    int sw = MeasureText(sub, 14);
    DrawText(sub, SCREEN_W/2 - sw/2, 60, 14, {85,85,130,255});

    // Title besar
    const char* title = "PERMAINAN SELESAI";
    int tw = MeasureText(title, 52);
    DrawText(title, SCREEN_W/2 - tw/2, 84, 52, WHITE);

    // Garis dekorasi
    DrawLine(SCREEN_W/2 - 300, 152, SCREEN_W/2 + 300, 152, {58,61,92,255});
}

// ─── drawRankingMaxTurn ───────────────────────────────────────────────────────
// Tampilkan semua pemain dengan ranking lengkap
void WinScreen::drawRankingMaxTurn() {
    float cardW  = 680;
    float cardH  = 80;
    float startX = SCREEN_W/2.f - cardW/2.f;
    float startY = 175;
    float gap    = 10;

    for (int i = 0; i < (int)results.size(); i++) {
        auto& r   = results[i];
        float cy  = startY + i*(cardH + gap);

        // Background card
        Color bgC  = r.isWinner ? Color{20,22,14,255} : Color{18,19,32,255};
        Color brdC = r.bankrupt ? Color{80,20,20,255}
                   : r.isWinner ? Color{(unsigned char)(200*(0.7f+0.3f*sinf(glowTimer*2.f))),
                                        (unsigned char)(160*(0.7f+0.3f*sinf(glowTimer*2.f))),
                                        0,255}
                                : Color{42,45,72,255};
        float brdW = r.isWinner ? 2.f : 1.f;

        DrawRectangle((int)startX, (int)cy, (int)cardW, (int)cardH, bgC);
        DrawRectangleLinesEx({startX, cy, cardW, cardH}, brdW, brdC);

        // Rank number
        std::string rankStr = r.bankrupt ? "-" : std::to_string(r.rank);
        Color rankCol = (r.rank==1)  ? Color{239,159,39,255}
                      : (r.rank==2)  ? Color{150,150,150,255}
                      : (r.rank==3)  ? Color{140,80,40,255}
                      : Color{80,80,100,255};
        if (r.bankrupt) rankCol = {100,30,30,255};
        int rw = MeasureText(rankStr.c_str(), 32);
        DrawText(rankStr.c_str(), (int)(startX+30-rw/2), (int)(cy+22), 32, rankCol);

        // Dot warna player
        DrawCircle((int)(startX+70), (int)(cy+cardH/2), 10,
                   r.bankrupt ? Color{60,60,80,255} : r.color);

        // Username
        DrawText(r.username.c_str(), (int)(startX+90), (int)(cy+14), 18,
                 r.bankrupt ? Color{80,80,100,255} : WHITE);

        // Stats: uang, properti, kartu
        if (!r.bankrupt) {
            std::string stats = "M " + std::to_string(r.money) +
                                "   " + std::to_string(r.propertyCount) + " properti" +
                                "   " + std::to_string(r.cardCount) + " kartu";
            DrawText(stats.c_str(), (int)(startX+90), (int)(cy+40), 13, {120,120,160,255});
        } else {
            DrawText("BANGKRUT", (int)(startX+90), (int)(cy+40), 13, {150,40,40,255});
        }

        // Winner badge
        if (r.isWinner) {
            const char* badge = "MENANG";
            int bw = MeasureText(badge, 11);
            float bx = startX + cardW - bw - 24;
            DrawRectangle((int)(bx-8), (int)(cy+28), bw+16, 24, {30,25,5,255});
            DrawRectangleLinesEx({bx-8, cy+28, (float)(bw+16), 24}, 1, {239,159,39,255});
            DrawText(badge, (int)bx, (int)(cy+34), 11, {239,159,39,255});
        }

        // Tiebreaker label — kalau rank sama dengan sebelumnya
        if (i > 0 && !r.bankrupt && r.rank == results[i-1].rank) {
            DrawText("SERI", (int)(startX+cardW-80), (int)(cy+14), 10, {100,100,140,255});
        }
    }
}

// ─── drawRankingBankruptcy ────────────────────────────────────────────────────
// Skenario bankruptcy: 1 pemenang, tampilkan besar di tengah
void WinScreen::drawRankingBankruptcy() {
    // Cari pemenang (satu-satunya yang tidak bankrupt)
    PlayerResult* winner = nullptr;
    for (auto& r : results)
        if (!r.bankrupt) { winner = &r; break; }

    if (!winner) return;

    // Nama pemenang besar di tengah
    float pulse = 0.8f + 0.2f * sinf(glowTimer * 2.f);
    Color nameC = {(unsigned char)(winner->color.r * pulse),
                   (unsigned char)(winner->color.g * pulse),
                   (unsigned char)(winner->color.b * pulse), 255};

    int nw = MeasureText(winner->username.c_str(), 64);
    DrawText(winner->username.c_str(), SCREEN_W/2 - nw/2, 200, 64, nameC);

    const char* sub = "PEMENANG";
    int sw = MeasureText(sub, 18);
    DrawText(sub, SCREEN_W/2-sw/2, 280, 18, {239,159,39,255});

    // Stats pemenang
    std::string stats = "M " + std::to_string(winner->money) +
                        "   " + std::to_string(winner->propertyCount) + " properti";
    int stw = MeasureText(stats.c_str(), 16);
    DrawText(stats.c_str(), SCREEN_W/2-stw/2, 315, 16, {120,120,160,255});

    // List yang bankrupt di bawah
    DrawLine(SCREEN_W/2-200, 360, SCREEN_W/2+200, 360, {42,45,72,255});
    float by = 376;
    for (auto& r : results) {
        if (r.bankrupt) {
            int rw = MeasureText(r.username.c_str(), 14);
            DrawText(r.username.c_str(), SCREEN_W/2-rw/2, (int)by, 14, {80,80,100,255});
            int bw = MeasureText("BANGKRUT", 11);
            DrawText("BANGKRUT", SCREEN_W/2-bw/2, (int)(by+18), 11, {120,40,40,255});
            by += 48;
        }
    }
}

// ─── drawButtons ─────────────────────────────────────────────────────────────
void WinScreen::drawButtons() {
    float btnY = SCREEN_H - 100;
    float btnH = 46;

    // MAIN LAGI
    Rectangle r1 = {SCREEN_W/2.f - 330, btnY, 200, btnH};
    bool h1 = CheckCollisionPointRec(GetMousePosition(), r1);
    float pulse = 0.7f + 0.3f * sinf(glowTimer * 2.5f);
    DrawRectangleRec(r1, h1? Color{30,30,60,255}:Color{20,21,40,255});
    DrawRectangleLinesEx(r1, h1?2.f:1.5f,
        {(unsigned char)(124*pulse),(unsigned char)(111*pulse),255,255});
    int w1 = MeasureText("MAIN LAGI", 14);
    DrawText("MAIN LAGI", (int)(r1.x+100-w1/2), (int)(btnY+15), 14,
             h1?WHITE:Color{180,170,255,255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && h1) wantsMainMenu = true;

    // MENU UTAMA
    Rectangle r2 = {SCREEN_W/2.f - 100, btnY, 200, btnH};
    bool h2 = CheckCollisionPointRec(GetMousePosition(), r2);
    DrawRectangleRec(r2, h2? Color{25,27,45,255}:Color{20,21,40,255});
    DrawRectangleLinesEx(r2, 1.f, {58,61,92,255});
    int w2 = MeasureText("MENU UTAMA", 14);
    DrawText("MENU UTAMA", (int)(r2.x+100-w2/2), (int)(btnY+15), 14,
             h2?WHITE:Color{120,120,160,255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && h2) wantsMainMenu = true;

    // KELUAR
    Rectangle r3 = {SCREEN_W/2.f + 130, btnY, 200, btnH};
    bool h3 = CheckCollisionPointRec(GetMousePosition(), r3);
    DrawRectangleRec(r3, h3? Color{40,15,15,255}:Color{20,21,40,255});
    DrawRectangleLinesEx(r3, 1.f, {100,30,30,255});
    int w3 = MeasureText("KELUAR", 14);
    DrawText("KELUAR", (int)(r3.x+100-w3/2), (int)(btnY+15), 14,
             h3?Color{255,80,80,255}:Color{150,50,50,255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && h3) wantsExit = true;
}