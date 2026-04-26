#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Player/Player.hpp"
#include <string>

extern const TileDef TILE_DEFS[40];

// ─── Left panel ───────────────────────────────────────────────────────────────
void GameScreen::drawLeftPanel()
{
    DrawRectangle(0, 0, LEFT_PANEL, SCREEN_H, {30, 30, 40, 255});
    DrawRectangleLinesEx({0, 0, (float)LEFT_PANEL, (float)SCREEN_H}, 1, {60, 60, 80, 255});
    DrawText("PEMAIN", 12, 14, 14, {150, 150, 180, 255});
    DrawLine(8, 32, LEFT_PANEL - 8, 32, {60, 60, 80, 255});

    float panelH = (SCREEN_H - 50.f) / 4.f;

    for (int p = 0; p < (int)gameState.players.size(); p++)
    {
        auto &pl = gameState.players[p];
        Color col = playerColors[p];
        float py = 40 + p * panelH;
        bool active = (p == gameState.activePlayerIdx);

        Rectangle card = {8, py, LEFT_PANEL - 16, panelH - 6};
        DrawRectangleRec(card, active ? Color{45, 50, 65, 255} : Color{35, 37, 48, 255});
        DrawRectangleLinesEx(card, active ? 2.f : 1.f, active ? col : Color{60, 60, 80, 255});
        DrawRectangle(8, (int)py, 4, (int)(panelH - 6), col);

        DrawCircle(22, (int)(py + 12), 10, col);
        std::string pn = std::to_string(p + 1);
        DrawText(pn.c_str(), 18, (int)(py + 6), 11, WHITE);

        DrawText(pl.username.c_str(), 38, (int)(py + 6), 13, WHITE);
        if (active)
            DrawText("GILIRAN", LEFT_PANEL - 90, (int)(py + 6), 10, col);

        DrawText(("M " + std::to_string(pl.money)).c_str(), 38, (int)(py + 22), 12, {100, 220, 100, 255});
        DrawText(("@ " + std::string(TILE_DEFS[pl.position].code)).c_str(),
                 38, (int)(py + 36), 11, {180, 180, 180, 255});

        Color sc = {100, 220, 100, 255};
        if (pl.status == "JAILED")
            sc = {220, 100, 100, 255};
        if (pl.status == "BANKRUPT")
            sc = {120, 120, 120, 255};
        DrawText(pl.status.c_str(), 38, (int)(py + 50), 10, sc);

        for (int c = 0; c < pl.cardCount && c < 3; c++)
        {
            DrawRectangle(38 + c * 16, (int)(py + 64), 12, 16, {80, 100, 160, 255});
            DrawRectangleLinesEx({38.f + c * 16, py + 64, 12, 16}, 1, {120, 140, 200, 255});
        }
        if (pl.cardCount > 0)
            DrawText(("x" + std::to_string(pl.cardCount)).c_str(),
                     38 + pl.cardCount * 16 + 2, (int)(py + 66), 10, {150, 150, 200, 255});

        if (p >= activePlayerCount)
        {
            DrawRectangleRec(card, {0, 0, 0, 180});
            DrawText("+", (int)(card.x + card.width / 2 - 5),
                     (int)(card.y + card.height / 2 - 6), 14, {50, 52, 80, 255});
        }
    }
}

// ─── Right panel ──────────────────────────────────────────────────────────────
void GameScreen::drawRightPanel()
{
    float rx = SCREEN_W - RIGHT_PANEL;
    DrawRectangle((int)rx, 0, RIGHT_PANEL, SCREEN_H, {30, 30, 40, 255});
    DrawRectangleLinesEx({rx, 0, (float)RIGHT_PANEL, (float)SCREEN_H}, 1, {60, 60, 80, 255});

    DrawText("GILIRAN", (int)rx + 10, 14, 14, {150, 150, 180, 255});
    DrawLine((int)rx + 8, 32, SCREEN_W - 8, 32, {60, 60, 80, 255});

    std::string ts = std::to_string(gameState.currentTurn) + " / " +
                     std::to_string(gameState.maxTurn);
    DrawText("Turn", (int)rx + 10, 38, 11, {150, 150, 180, 255});
    DrawText(ts.c_str(), (int)rx + 10, 52, 16, WHITE);

    auto &curP = gameState.players[gameState.activePlayerIdx];
    DrawText("Giliran:", (int)rx + 10, 76, 11, {150, 150, 180, 255});
    DrawText(curP.username.c_str(), (int)rx + 10, 90, 14, playerColors[gameState.activePlayerIdx]);

    // Tambah di drawRightPanel(), setelah DrawText("MODE: REAL"...)
    const char *phaseStr = "";
    if (isRealMode())
    {
        auto ph = guiManager->getGameMaster()->getState().getPhase();
        switch (ph)
        {
        case GamePhase::PLAYER_TURN:
            phaseStr = "PLAYER_TURN";
            break;
        case GamePhase::AWAITING_BUY:
            phaseStr = "AWAITING_BUY";
            break;
        case GamePhase::AWAITING_TAX:
            phaseStr = "AWAITING_TAX";
            break;
        case GamePhase::AWAITING_FESTIVAL:
            phaseStr = "AWAITING_FESTIVAL";
            break;
        case GamePhase::AWAITING_DROP_SKILL_CARD:
            phaseStr = "AWAITING_DROP_SKILL_CARD";
            break;
        case GamePhase::SHOW_CARD:
            phaseStr = "SHOW_CARD";
            break;
        case GamePhase::AUCTION:
            phaseStr = "AUCTION";
            break;
        case GamePhase::BANKRUPTCY:
            phaseStr = "BANKRUPTCY";
            break;
        case GamePhase::GAME_OVER:
            phaseStr = "GAME_OVER";
            break;
        default:
            phaseStr = "OTHER";
            break;
        }
        // Info hutang saat BANKRUPTCY
        if (ph == GamePhase::BANKRUPTCY)
        {
            const GameState &gs = guiManager->getGameMaster()->getState();
            int debt = gs.getPendingDebt();
            Player *cred = gs.getPendingCreditor();
            std::string credName = cred ? cred->getUsername() : "Bank";
            std::string debtInfo = "Hutang: M" + std::to_string(debt);
            std::string credInfo = "ke: " + credName;
            DrawText(debtInfo.c_str(), (int)rx + 10, SCREEN_H - 65, 11, {220, 100, 100, 255});
            DrawText(credInfo.c_str(), (int)rx + 10, SCREEN_H - 50, 11, {200, 140, 140, 255});
        }
    }
    DrawText(phaseStr, (int)rx + 10, SCREEN_H - 35, 10, {200, 200, 100, 255});

    DrawLine((int)rx + 8, 112, SCREEN_W - 8, 112, {60, 60, 80, 255});
    DrawText("AKSI", (int)rx + 10, 120, 12, {150, 150, 180, 255});

    bool hasExtraTurn = false;
    if (isRealMode())
        hasExtraTurn = guiManager->getGameMaster()->getState().getHasExtraTurn();

    class Btn
    {
    public:
        const char *label;
        Color col;
    };
    Btn btns[] = {
        {"LEMPAR DADU", {70, 130, 180, 255}},
        {"ATUR DADU", {180, 100, 220, 255}},
        {"USE CARD", {100, 160, 100, 255}},
        {"BELI BANGUNAN", {80, 140, 80, 255}},
        {"GADAI", {180, 130, 60, 255}},
        {"TEBUS", {160, 120, 80, 255}},
        {"JUAL BANGUNAN", {160, 80, 80, 255}},
        {"SAVE", {100, 100, 160, 255}},
        {hasExtraTurn ? "START EXTRA TURN" : "END TURN", {80, 100, 160, 255}},
    };

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < 9; i++)
    {
        Rectangle btn = {rx + 10, 140.f + i * 40, RIGHT_PANEL - 20, 32};
        bool hover = CheckCollisionPointRec(mouse, btn);
        bool disabled = false;

        if (isRealMode())
        {
            GameState &gs = guiManager->getGameMaster()->getState();

            GamePhase phase = gs.getPhase();
            bool hasRolled = gs.getHasRolled();
            bool hasUsedCard = gs.getHasUsedCard();
            Player *cur = gs.getCurrPlayer();
            switch (i)
            {
            case 0:
            case 1:
                disabled = hasRolled || diceState.animating || phase != GamePhase::PLAYER_TURN;
                break;
            case 2:
                disabled =
                    phase != GamePhase::PLAYER_TURN ||
                    hasRolled ||
                    hasUsedCard ||
                    cur == nullptr ||
                    cur->getHandSize() == 0;
                break;
            case 3:
                disabled = phase != GamePhase::PLAYER_TURN;
                break;
            case 4:
            case 5:
            case 6:
                disabled = (phase == GamePhase::AUCTION ||
                            phase == GamePhase::BANKRUPTCY ||
                            phase == GamePhase::GAME_OVER);
                break;
            case 7:
                disabled = hasRolled || phase != GamePhase::PLAYER_TURN;
                break;
            case 8:
                disabled = !hasRolled || phase != GamePhase::PLAYER_TURN;
                break;
            default:
                break;
            }
        }
        else
        {
            if (i == 0 || i == 1)
                disabled = diceState.hasRolled || diceState.animating;
        }

        Color colBase = disabled ? Color{50, 50, 60, 255} : Color{40, 42, 54, 255};
        Color colHov = disabled ? Color{50, 50, 60, 255}
                                : Color{btns[i].col.r, btns[i].col.g, btns[i].col.b, 220};
        Color border = disabled ? Color{70, 70, 80, 255} : btns[i].col;

        DrawRectangleRec(btn, hover && !disabled ? colHov : colBase);
        DrawRectangleLinesEx(btn, 1, border);
        int tw = MeasureText(btns[i].label, 11);
        Color textCol = disabled ? Color{90, 90, 100, 255}
                                 : (hover ? WHITE : Color{200, 200, 210, 255});
        DrawText(btns[i].label, (int)(rx + RIGHT_PANEL / 2 - tw / 2),
                 (int)(140 + i * 40 + 10), 11, textCol);

        if (!disabled && hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            switch (i)
            {
            case 0:
                handleLemparDadu();
                break;
            case 1:
                triggerAturDaduDialog();
                break;
            case 2:
                triggerSkillCardDialog();
                break;
            case 3:
                triggerBangunDialog();
                break;
            case 4:
                triggerGadaiDialog();
                break;
            case 5:
                triggerTebusDialog();
                break;
            case 6:
                triggerJualBangunanDialog();
                break;
            case 7:
                handleSimpan();
                break;
            case 8:

                if (guiManager && guiManager->getGameMaster())
                {
                    GameMaster *gm = guiManager->getGameMaster();
                    gm->endTurn();
                    gm->beginTurn();
                }
                break;
            default:
                break;
            }
        }
    }

    const char *modeStr = isRealMode() ? "MODE: REAL" : "MODE: MOCK";
    Color modeCol = isRealMode() ? Color{100, 220, 100, 255} : Color{220, 100, 100, 255};
    DrawText(modeStr, (int)rx + 10, SCREEN_H - 20, 11, modeCol);

    // ─── FOOTER BUTTONS ──────────────────────────────────────────────────────
    float footerY = SCREEN_H - 140;
    DrawLine((int)rx + 8, (int)footerY, (int)SCREEN_W - 8, (int)footerY, {60, 60, 80, 255});

    // 1. CETAK AKTA
    Rectangle aktaBtn = {rx + 10, footerY + 10, RIGHT_PANEL - 20, 28};
    DrawRectangleRec(aktaBtn, {40, 42, 54, 255});
    DrawRectangleLinesEx(aktaBtn, 1, {220, 190, 90, 255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), aktaBtn))
        triggerAktaDialog();
    int aw = MeasureText("CETAK AKTA", 10);
    DrawText("CETAK AKTA", (int)(rx + RIGHT_PANEL / 2 - aw / 2), (int)footerY + 19, 10, {220, 190, 90, 255});

    // 2. CETAK PROPERTI
    Rectangle propBtn = {rx + 10, footerY + 44, RIGHT_PANEL - 20, 28};
    DrawRectangleRec(propBtn, {40, 42, 54, 255});
    DrawRectangleLinesEx(propBtn, 1, {80, 130, 200, 255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), propBtn))
        triggerCetakPropertiDialog(gameState.activePlayerIdx);
    int pw = MeasureText("CETAK PROPERTI", 10);
    DrawText("CETAK PROPERTI", (int)(rx + RIGHT_PANEL / 2 - pw / 2), (int)footerY + 53, 10, {100, 160, 220, 255});

    // 3. LOG TRANSAKSI
    Rectangle logBtn = {rx + 10, footerY + 78, RIGHT_PANEL - 20, 32};
    DrawRectangleRec(logBtn, {40, 42, 54, 255});
    DrawRectangleLinesEx(logBtn, 1, {100, 100, 160, 255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), logBtn))
        showLogPopup = !showLogPopup;
    int lw = MeasureText("LOG TRANSAKSI", 11);
    DrawText("LOG TRANSAKSI", (int)(rx + RIGHT_PANEL / 2 - lw / 2), (int)footerY + 88, 11, {150, 150, 200, 255});

    // 4. MAIN MENU
    Rectangle menuBtn = {rx + 10, footerY + 112, RIGHT_PANEL - 20, 28};
    DrawRectangleRec(menuBtn, {40, 20, 20, 255});
    DrawRectangleLinesEx(menuBtn, 1, {180, 60, 60, 255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), menuBtn))
        mainMenuConfirm.visible = true;
    int mw = MeasureText("MAIN MENU", 10);
    DrawText("MAIN MENU", (int)(rx + RIGHT_PANEL / 2 - mw / 2), (int)footerY + 121, 10, {220, 80, 80, 255});
}