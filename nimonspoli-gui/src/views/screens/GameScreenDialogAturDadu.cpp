#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/Commands/AturDaduCommand.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include <string>

void GameScreen::triggerAturDaduDialog()
{
    aturDaduDialog.visible = true;
    aturDaduDialog.input1 = "1";
    aturDaduDialog.input2 = "1";
    aturDaduDialog.focusField = 1;
    aturDaduDialog.errorMsg = "";
}

void GameScreen::drawAturDaduDialog()
{
    if (!aturDaduDialog.visible) return;

    // Overlay
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150});

    float boxW = 400, boxH = 300;
    float boxX = SCREEN_W / 2 - boxW / 2;
    float boxY = SCREEN_H / 2 - boxH / 2;

    DrawRectangleRounded({boxX, boxY, boxW, boxH}, 0.1f, 8, {30, 30, 40, 255});
    //DrawRectangleRoundedLines({boxX, boxY, boxW, boxH}, 0.1f, 8, 2, {100, 100, 250, 255});

    DrawText("ATUR DADU MANUAL", (int)boxX + 20, (int)boxY + 20, 20, WHITE);
    DrawLine((int)boxX + 20, (int)boxY + 50, (int)boxX + boxW - 20, (int)boxY + 50, {80, 80, 100, 255});

    // Input Dadu 1
    DrawText("Dadu 1 (1-6):", (int)boxX + 40, (int)boxY + 80, 16, {180, 180, 200, 255});
    Rectangle r1 = {boxX + 40, boxY + 105, 320, 40};
    DrawRectangleRec(r1, aturDaduDialog.focusField == 1 ? Color{50, 52, 70, 255} : Color{40, 42, 54, 255});
    DrawRectangleLinesEx(r1, 1, aturDaduDialog.focusField == 1 ? SKYBLUE : GRAY);
    DrawText(aturDaduDialog.input1.c_str(), (int)r1.x + 10, (int)r1.y + 10, 20, WHITE);
    if (aturDaduDialog.focusField == 1 && (int)(GetTime() * 2) % 2 == 0) {
        int tw = MeasureText(aturDaduDialog.input1.c_str(), 20);
        DrawLine((int)r1.x + 12 + tw, (int)r1.y + 8, (int)r1.x + 12 + tw, (int)r1.y + 32, WHITE);
    }
    if (CheckCollisionPointRec(GetMousePosition(), r1) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        aturDaduDialog.focusField = 1;

    // Input Dadu 2
    DrawText("Dadu 2 (1-6):", (int)boxX + 40, (int)boxY + 160, 16, {180, 180, 200, 255});
    Rectangle r2 = {boxX + 40, boxY + 185, 320, 40};
    DrawRectangleRec(r2, aturDaduDialog.focusField == 2 ? Color{50, 52, 70, 255} : Color{40, 42, 54, 255});
    DrawRectangleLinesEx(r2, 1, aturDaduDialog.focusField == 2 ? SKYBLUE : GRAY);
    DrawText(aturDaduDialog.input2.c_str(), (int)r2.x + 10, (int)r2.y + 10, 20, WHITE);
    if (aturDaduDialog.focusField == 2 && (int)(GetTime() * 2) % 2 == 0) {
        int tw = MeasureText(aturDaduDialog.input2.c_str(), 20);
        DrawLine((int)r2.x + 12 + tw, (int)r2.y + 8, (int)r2.x + 12 + tw, (int)r2.y + 32, WHITE);
    }
    if (CheckCollisionPointRec(GetMousePosition(), r2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        aturDaduDialog.focusField = 2;

    if (!aturDaduDialog.errorMsg.empty()) {
        DrawText(aturDaduDialog.errorMsg.c_str(), (int)boxX + 40, (int)boxY + 230, 12, RED);
    }

    // Buttons
    Rectangle okBtn = {boxX + 40, boxY + 250, 150, 32};
    bool okHover = CheckCollisionPointRec(GetMousePosition(), okBtn);
    DrawRectangleRec(okBtn, okHover ? Color{60, 180, 60, 255} : Color{40, 140, 40, 255});
    DrawText("KONFIRMASI", (int)okBtn.x + 25, (int)okBtn.y + 8, 14, WHITE);

    Rectangle cancelBtn = {boxX + boxW - 190, boxY + 250, 150, 32};
    bool cancelHover = CheckCollisionPointRec(GetMousePosition(), cancelBtn);
    DrawRectangleRec(cancelBtn, cancelHover ? Color{180, 60, 60, 255} : Color{140, 40, 40, 255});
    DrawText("BATAL", (int)cancelBtn.x + 50, (int)cancelBtn.y + 8, 14, WHITE);

    // Keyboard handling
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= '1' && key <= '6') {
            if (aturDaduDialog.focusField == 1) aturDaduDialog.input1 = (char)key;
            else if (aturDaduDialog.focusField == 2) aturDaduDialog.input2 = (char)key;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (aturDaduDialog.focusField == 1) aturDaduDialog.input1 = "";
        else if (aturDaduDialog.focusField == 2) aturDaduDialog.input2 = "";
    }
    if (IsKeyPressed(KEY_TAB)) {
        aturDaduDialog.focusField = (aturDaduDialog.focusField == 1) ? 2 : 1;
    }

    if (IsKeyPressed(KEY_ENTER) || (okHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        try {
            int v1 = std::stoi(aturDaduDialog.input1);
            int v2 = std::stoi(aturDaduDialog.input2);
            if (v1 < 1 || v1 > 6 || v2 < 1 || v2 > 6) {
                aturDaduDialog.errorMsg = "Nilai harus 1-6!";
            } else {
                if (guiManager && guiManager->getGameMaster()) {
                    GameMaster* gm = guiManager->getGameMaster();
                    Player* p = gm->getState().getCurrPlayer();
                    Dice* d = gm->getState().getDice();
                    guiManager->pushCommand(new AturDaduCommand(*gm, p, *d, v1, v2));
                }
                aturDaduDialog.visible = false;
            }
        } catch (...) {
            aturDaduDialog.errorMsg = "Input tidak valid!";
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || (cancelHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        aturDaduDialog.visible = false;
    }
}
