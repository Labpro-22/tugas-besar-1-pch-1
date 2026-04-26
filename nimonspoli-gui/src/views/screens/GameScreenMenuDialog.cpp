#include "GameScreen.hpp"
#include <ctime>
#include <time.h>
void GameScreen::drawMainMenuConfirm() {
    if (!mainMenuConfirm.visible) return;

    Vector2 mouse = {
        GetMousePosition().x * (1920.f / GetScreenWidth()),
        GetMousePosition().y * (1080.f / GetScreenHeight())
    };

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 420.f, PH = 200.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, {180, 60, 60, 255});

    // Header
    DrawRectangle((int)px, (int)py, (int)PW, 48, {120, 30, 30, 255});
    const char* title = "KEMBALI KE MAIN MENU?";
    int tw = MeasureText(title, 14);
    DrawText(title, (int)(px + PW/2 - tw/2), (int)(py + 17), 14, WHITE);

    DrawText("Game yang belum disimpan akan hilang.",
             (int)(px + 20), (int)(py + 62), 12, {180, 180, 210, 255});

    float btnY = py + PH - 60;
    float btnW = (PW - 64) / 3.f;

    // SAVE & KELUAR
    Rectangle saveBtn = {px + 12, btnY, btnW, 44};
    bool saveHov = CheckCollisionPointRec(mouse, saveBtn);
    DrawRectangleRec(saveBtn, saveHov ? Color{40,120,60,255} : Color{28,80,45,255});
    DrawRectangleLinesEx(saveBtn, 1.5f, {80, 200, 100, 255});
    int sw = MeasureText("SAVE & KELUAR", 10);
    DrawText("SAVE &", (int)(saveBtn.x + btnW/2 - MeasureText("SAVE &", 10)/2),
             (int)(btnY + 8), 10, WHITE);
    DrawText("KELUAR", (int)(saveBtn.x + btnW/2 - MeasureText("KELUAR", 10)/2),
             (int)(btnY + 22), 10, WHITE);

    // KELUAR TANPA SAVE
    Rectangle exitBtn = {px + 16 + btnW + 4, btnY, btnW, 44};
    bool exitHov = CheckCollisionPointRec(mouse, exitBtn);
    DrawRectangleRec(exitBtn, exitHov ? Color{180,60,60,255} : Color{120,40,40,255});
    DrawRectangleLinesEx(exitBtn, 1.5f, {220, 80, 80, 255});
    DrawText("KELUAR", (int)(exitBtn.x + btnW/2 - MeasureText("KELUAR", 10)/2),
             (int)(btnY + 8), 10, WHITE);
    DrawText("TANPA SAVE", (int)(exitBtn.x + btnW/2 - MeasureText("TANPA SAVE", 10)/2),
             (int)(btnY + 22), 10, WHITE);

    // BATAL
    Rectangle cancelBtn = {px + 20 + 2*btnW + 8, btnY, btnW, 44};
    bool cancelHov = CheckCollisionPointRec(mouse, cancelBtn);
    DrawRectangleRec(cancelBtn, cancelHov ? Color{60,60,80,255} : Color{40,42,58,255});
    DrawRectangleLinesEx(cancelBtn, 1.5f, {100, 100, 140, 255});
    int cw = MeasureText("BATAL", 11);
    DrawText("BATAL", (int)(cancelBtn.x + btnW/2 - cw/2), (int)(btnY + 16), 11, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (saveHov) {
            // Save otomatis dengan timestamp agar tidak bentrok, lalu keluar
            time_t now = time(0);
            tm *ltm = localtime(&now);
            char buf[64];
            strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", ltm);
            std::string autoName = "data/autosave_" + std::string(buf) + ".txt";
            
            doSave(autoName);
            
            wantsMainMenu = true;
            mainMenuConfirm.visible = false;
        } else if (exitHov) {
            wantsMainMenu = true;
            mainMenuConfirm.visible = false;
        } else if (cancelHov) {
            mainMenuConfirm.visible = false;
        }
    }
}