#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Player/Player.hpp"
#include "../../core/GameState/GameState.hpp"
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  triggerPbmDialog
//  Dipanggil dari syncFromGameMaster() saat phase == AWAITING_PBM.
//  PBM sudah dieksekusi di TaxTile::onLanded() — dialog ini hanya info.
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::triggerPbmDialog()
{
    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *cur = gs.getCurrPlayer();
    if (!cur)
        return;

    int amount = gs.getPendingPbmAmount();

    pbmDialog.amount = amount;
    pbmDialog.balanceAfter = cur->getBalance();
    pbmDialog.balanceBefore = cur->getBalance() + amount; // sudah dipotong
    pbmDialog.visible = true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawPbmDialog
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::drawPbmDialog()
{
    if (!pbmDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 420.f, PH = 260.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, {180, 80, 60, 255});

    // Header
    DrawRectangle((int)px, (int)py, (int)PW, 52, {160, 50, 30, 255});
    const char *title = "PAJAK BARANG MEWAH(PBM)";
    int tw = MeasureText(title, 14);
    DrawText(title, (int)(px + PW / 2 - tw / 2), (int)(py + 18), 14, WHITE);

    float ry = py + 72;
    float lx = px + 24;
    float rx = px + PW - 24;

    // Info
    DrawText("PBM telah dibayarkan ke Bank.", (int)lx, (int)ry, 12, {180, 180, 210, 255});
    ry += 30;

    DrawLine((int)(lx), (int)(ry), (int)(rx), (int)(ry), {60, 60, 90, 255});
    ry += 12;

    // Jumlah
    DrawText("Jumlah PBM", (int)lx, (int)ry, 12, {140, 140, 180, 255});
    std::string amtStr = "M" + std::to_string(pbmDialog.amount);
    int aw = MeasureText(amtStr.c_str(), 16);
    DrawText(amtStr.c_str(), (int)(rx - aw), (int)(ry - 2), 16, {255, 200, 60, 255});
    ry += 30;

    // Saldo
    DrawText("Saldo kamu", (int)lx, (int)ry, 12, {140, 140, 180, 255});
    std::string balStr = "M" + std::to_string(pbmDialog.balanceBefore) +
                         " -> M" + std::to_string(pbmDialog.balanceAfter);
    int bw = MeasureText(balStr.c_str(), 13);
    DrawText(balStr.c_str(), (int)(rx - bw), (int)(ry - 1), 13, {200, 240, 200, 255});

    // Tombol OK
    float btnW = 120.f;
    Rectangle okBtn = {px + PW / 2 - btnW / 2, py + PH - 52, btnW, 38};
    bool okHov = CheckCollisionPointRec(GetMousePosition(), okBtn);
    DrawRectangleRec(okBtn, okHov ? Color{80, 160, 100, 255} : Color{50, 110, 70, 255});
    DrawRectangleLinesEx(okBtn, 1.5f, {100, 200, 130, 255});
    int okw = MeasureText("OK", 14);
    DrawText("OK", (int)(okBtn.x + btnW / 2 - okw / 2), (int)(okBtn.y + 11), 14, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && okHov && isRealMode())
    {
        GameMaster* gm = guiManager->getGameMaster();
        GameState& gs = gm->getState();
        if (gs.getPendingDebt() > 0)
        {
            gs.setPhase(GamePhase::BANKRUPTCY);
        }
        else
        {
            gs.setPhase(GamePhase::PLAYER_TURN);
        }
        pbmDialog.visible = false;
    }
}