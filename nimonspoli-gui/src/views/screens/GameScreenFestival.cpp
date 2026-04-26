#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/Commands/FestivalCommand.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include <algorithm>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  triggerFestivalDialog
//  Dipanggil dari syncFromGameMaster() saat phase == AWAITING_FESTIVAL.
//  Selalu menampilkan dialog — jika tidak ada properti eligible, dialog
//  menampilkan pesan info dan tombol OK untuk lanjut.
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::triggerFestivalDialog()
{
    if (!isRealMode())
        return;
    const GameState &gs = guiManager->getGameMaster()->getState();
    Player *curP = gs.getCurrPlayer();
    if (!curP)
        return;

    festivalDialog.streets.clear();
    // Dapatkan daftar properti eligible via FestivalCommand helper
    FestivalCommand cmd(curP, gs.getLogger(), gs.getCurrTurn());
    festivalDialog.streets = cmd.getEligibleStreets();

    festivalDialog.scrollY = 0.f;
    festivalDialog.hoveredIdx = -1;
    festivalDialog.visible = true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawFestivalDialog
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::drawFestivalDialog()
{
    if (!festivalDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 500.f, PH = 480.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, {239, 159, 39, 255});

    DrawRectangle((int)px, (int)py, (int)PW, 52, {180, 110, 20, 255});
    const char *title = "FESTIVAL";
    int tw = MeasureText(title, 18);
    DrawText(title, (int)(px + PW / 2 - tw / 2), (int)(py + 16), 18, WHITE);

    // ── Kasus: tidak ada properti eligible ────────────────────────────────
    if (festivalDialog.streets.empty())
    {
        // Pesan info
        float msgY = py + 100;
        const char *line1 = "Kamu mendarat di petak Festival,";
        const char *line2 = "tapi tidak memiliki properti lahan.";
        const char *line3 = "Efek festival tidak dapat diterapkan.";
        int w1 = MeasureText(line1, 13), w2 = MeasureText(line2, 13), w3 = MeasureText(line3, 11);
        DrawText(line1, (int)(px + PW / 2 - w1 / 2), (int)msgY, 13, {200, 200, 220, 255});
        DrawText(line2, (int)(px + PW / 2 - w2 / 2), (int)(msgY + 22), 13, {200, 200, 220, 255});
        DrawText(line3, (int)(px + PW / 2 - w3 / 2), (int)(msgY + 52), 11, {150, 150, 180, 255});

        // Tombol OK
        float btnW = 120.f;
        Rectangle okBtn = {px + PW / 2 - btnW / 2, py + PH - 70, btnW, 42};
        bool okHov = CheckCollisionPointRec(GetMousePosition(), okBtn);
        DrawRectangleRec(okBtn, okHov ? Color{80, 160, 100, 255} : Color{50, 110, 70, 255});
        DrawRectangleLinesEx(okBtn, 1.5f, {100, 200, 130, 255});
        int okw = MeasureText("OK", 14);
        DrawText("OK", (int)(okBtn.x + btnW / 2 - okw / 2), (int)(okBtn.y + 13), 14, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && okHov && isRealMode())
        {
            guiManager->getGameMaster()->getState().setPhase(GamePhase::PLAYER_TURN);
            festivalDialog.visible = false;
        }
        return;
    }

    // ── Kasus: ada properti eligible ─────────────────────────────────────
    DrawText("Pilih properti untuk mengaktifkan festival:",
             (int)(px + 16), (int)(py + 58), 11, {180, 180, 210, 255});

    float listX = px + 16, listY = py + 76, listW = PW - 32;
    float listH = PH - 80 - 56, rowH = 62.f;
    auto &streets = festivalDialog.streets;
    float totalH = streets.size() * rowH;

    Rectangle listArea = {listX, listY, listW, listH};
    if (CheckCollisionPointRec(GetMousePosition(), listArea))
    {
        float wheel = GetMouseWheelMove();
        festivalDialog.scrollY -= wheel * 40.f;
        festivalDialog.scrollY = std::max(0.f,
                                          std::min(festivalDialog.scrollY, std::max(0.f, totalH - listH)));
    }

    Vector2 mouse = GetMousePosition();
    BeginScissorMode((int)listX, (int)listY, (int)listW, (int)listH);
    for (int i = 0; i < (int)streets.size(); i++)
    {
        StreetProperty *sp = streets[i];
        float ry = listY + i * rowH - festivalDialog.scrollY;
        if (ry + rowH < listY || ry > listY + listH)
            continue;

        bool hov = CheckCollisionPointRec(mouse, {listX, ry, listW, rowH - 4});
        Color rowBg = hov ? Color{45, 50, 72, 255} : Color{30, 32, 48, 255};
        DrawRectangle((int)listX, (int)ry, (int)listW, (int)(rowH - 4), rowBg);
        DrawRectangleLinesEx({listX, ry, listW, rowH - 4}, 1.f,
                             hov ? Color{239, 159, 39, 255} : Color{60, 60, 90, 255});

        int mult = sp->getFestivalMultiplier();
        std::string multStr = "x" + std::to_string(mult);
        Color multCol = (mult == 1)   ? Color{100, 100, 140, 255}
                        : (mult == 2) ? Color{239, 159, 39, 255}
                        : (mult == 4) ? Color{186, 117, 23, 255}
                                      : Color{133, 79, 11, 255};
        DrawText(multStr.c_str(), (int)(listX + 10), (int)(ry + 10), 16, multCol);
        DrawText(sp->getCode().c_str(), (int)(listX + 50), (int)(ry + 8), 13, WHITE);
        DrawText(sp->getName().c_str(), (int)(listX + 50), (int)(ry + 26), 11, {180, 180, 200, 255});

        int rent = sp->calculateRentPrice(0, 1, false);
        std::string rentStr = "Sewa: M" + std::to_string(rent);
        int rw = MeasureText(rentStr.c_str(), 11);
        DrawText(rentStr.c_str(), (int)(listX + listW - rw - 12), (int)(ry + 8), 11, {100, 220, 100, 255});

        if (mult > 1)
        {
            std::string durStr = "Durasi: " + std::to_string(sp->getFestivalDuration()) + " giliran";
            int dw = MeasureText(durStr.c_str(), 10);
            DrawText(durStr.c_str(), (int)(listX + listW - dw - 12), (int)(ry + 26), 10, multCol);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hov && isRealMode())
        {
            GameMaster *gm = guiManager->getGameMaster();
            GameState &gs = gm->getState();
            Player *curP = gs.getCurrPlayer();
            if (curP)
            {
                FestivalCommand *cmd = new FestivalCommand(curP, gs.getLogger(), gs.getCurrTurn());
                cmd->executeWithProperty(sp);
                delete cmd;
            }
            gs.setPhase(GamePhase::PLAYER_TURN);
            festivalDialog.visible = false;
            EndScissorMode();
            return;
        }
    }
    EndScissorMode();

    // Scrollbar
    if (totalH > listH)
    {
        float sbH = listH * (listH / totalH);
        float sbY = listY + (festivalDialog.scrollY / totalH) * listH;
        DrawRectangle((int)(listX + listW - 5), (int)listY, 4, (int)listH, {40, 42, 58, 255});
        DrawRectangle((int)(listX + listW - 5), (int)sbY, 4, (int)sbH, {200, 140, 40, 255});
    }

    DrawText("Klik properti untuk mengaktifkan festival",
             (int)(px + 16), (int)(py + PH - 34), 10, {100, 100, 140, 255});
}