#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/Commands/BayarPajakCommand.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include <string>

void GameScreen::triggerTaxDialog()
{
    if (!isRealMode()) return;
    const GameState& gs = guiManager->getGameMaster()->getState();
    Player* curP = gs.getCurrPlayer();
    if (!curP) return;

    taxDialog.flatAmount    = gs.getPendingPphFlat();
    taxDialog.pctAmount     = gs.getPendingPphPct();
    taxDialog.wealth        = curP->getWealth();
    taxDialog.taxAmtPct     = (taxDialog.wealth * taxDialog.pctAmount) / 100;
    taxDialog.canAffordFlat = curP->canAfford(taxDialog.flatAmount);
    taxDialog.canAffordPct  = curP->canAfford(taxDialog.taxAmtPct);
    taxDialog.visible       = true;
}

void GameScreen::drawTaxDialog()
{
    if (!taxDialog.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    constexpr float PW = 460.f, PH = 340.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px,(int)py,(int)PW,(int)PH,{22,24,38,255});
    DrawRectangleLinesEx({px,py,PW,PH},2.f,{180,80,60,255});

    DrawRectangle((int)px,(int)py,(int)PW,52,{200,60,40,255});
    const char* title = "PAJAK PENGHASILAN (PPH)";
    int tw = MeasureText(title,16);
    DrawText(title,(int)(px+PW/2-tw/2),(int)(py+18),16,WHITE);

    float ry = py + 66;
    DrawText("Total kekayaan kamu:",(int)(px+20),(int)ry,12,{150,150,190,255});
    std::string wStr = "M" + std::to_string(taxDialog.wealth);
    int ww = MeasureText(wStr.c_str(),14);
    DrawText(wStr.c_str(),(int)(px+PW-20-ww),(int)(ry-2),14,{220,200,100,255});
    DrawLine((int)(px+16),(int)(ry+20),(int)(px+PW-16),(int)(ry+20),{60,60,90,255});
    ry += 30;

    DrawText("Pilih cara pembayaran pajak:",(int)(px+20),(int)ry,12,{180,180,210,255});
    DrawText("(Pilih SEBELUM menghitung kekayaan!)",(int)(px+20),(int)(ry+16),10,{120,120,160,255});

    Vector2 mouse = GetMousePosition();
    float btnW = (PW-48)/2.f;
    float btnY = py + PH - 70;

    // Tombol BAYAR FLAT
    Rectangle flatBtn = {px+16, btnY, btnW, 52};
    bool flatHov = CheckCollisionPointRec(mouse,flatBtn) && taxDialog.canAffordFlat;
    bool flatDis = !taxDialog.canAffordFlat;
    Color flatBg = flatDis ? Color{40,42,54,255}
                 : flatHov ? Color{60,170,90,255}
                           : Color{40,130,70,255};
    DrawRectangleRec(flatBtn,flatBg);
    DrawRectangleLinesEx(flatBtn,1.5f,flatDis?Color{60,60,80,255}:Color{80,210,110,255});
    std::string fl1s = "BAYAR FLAT";
    std::string fl2s = "M" + std::to_string(taxDialog.flatAmount);
    int fl1 = MeasureText(fl1s.c_str(),11), fl2 = MeasureText(fl2s.c_str(),16);
    DrawText(fl1s.c_str(),(int)(flatBtn.x+btnW/2-fl1/2),(int)(btnY+8),11,
             flatDis?Color{80,80,100,255}:WHITE);
    DrawText(fl2s.c_str(),(int)(flatBtn.x+btnW/2-fl2/2),(int)(btnY+24),16,
             flatDis?Color{80,80,100,255}:Color{220,255,220,255});
    if (flatDis) {
        const char* nd = "Tidak mampu"; int ndw = MeasureText(nd,9);
        DrawText(nd,(int)(flatBtn.x+btnW/2-ndw/2),(int)(btnY+42),9,{220,80,80,255});
    }

    // Tombol BAYAR PERSEN
    Rectangle pctBtn = {px+16+btnW+16, btnY, btnW, 52};
    bool pctHov = CheckCollisionPointRec(mouse,pctBtn) && taxDialog.canAffordPct;
    bool pctDis = !taxDialog.canAffordPct;
    Color pctBg = pctDis ? Color{40,42,54,255}
                : pctHov ? Color{60,110,200,255}
                         : Color{40,80,160,255};
    DrawRectangleRec(pctBtn,pctBg);
    DrawRectangleLinesEx(pctBtn,1.5f,pctDis?Color{60,60,80,255}:Color{100,150,240,255});
    std::string pl1s = std::to_string(taxDialog.pctAmount) + "% dari kekayaan";
    std::string pl2s = "M" + std::to_string(taxDialog.taxAmtPct);
    int pl1 = MeasureText(pl1s.c_str(),11), pl2 = MeasureText(pl2s.c_str(),16);
    DrawText(pl1s.c_str(),(int)(pctBtn.x+btnW/2-pl1/2),(int)(btnY+8),11,
             pctDis?Color{80,80,100,255}:WHITE);
    DrawText(pl2s.c_str(),(int)(pctBtn.x+btnW/2-pl2/2),(int)(btnY+24),16,
             pctDis?Color{80,80,100,255}:Color{200,220,255,255});
    if (pctDis) {
        const char* nd = "Tidak mampu"; int ndw = MeasureText(nd,9);
        DrawText(nd,(int)(pctBtn.x+btnW/2-ndw/2),(int)(btnY+42),9,{220,80,80,255});
    }

    // Handle klik
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isRealMode()) {
        GameMaster* gm  = guiManager->getGameMaster();
        GameState&  gs  = gm->getState();
        Player* player  = gs.getCurrPlayer();
        Bank*   bank    = gs.getBank();
        Board*  board   = gs.getBoard();
        int     pos     = player ? player->getPosition() : -1;
        Tile*   tile    = (board && pos >= 0) ? board->getTile(pos) : nullptr;
        TaxTile* taxTile = dynamic_cast<TaxTile*>(tile);

        if (flatHov && !flatDis && player && bank && taxTile) {
            BayarPajakCommand* cmd = new BayarPajakCommand(
                player,taxTile,bank,gs.getLogger(),gs.getTaxConfig(),gs.getCurrTurn());
            try { cmd->handlePPHChoice(1); }
            catch (...) { delete cmd; gs.setPhase(GamePhase::BANKRUPTCY); taxDialog.visible=false; return; }
            delete cmd;
            gs.setPhase(GamePhase::PLAYER_TURN);
            taxDialog.visible = false;
        } else if (pctHov && !pctDis && player && bank && taxTile) {
            BayarPajakCommand* cmd = new BayarPajakCommand(
                player,taxTile,bank,gs.getLogger(),gs.getTaxConfig(),gs.getCurrTurn());
            try { cmd->handlePPHChoice(2); }
            catch (...) { delete cmd; gs.setPhase(GamePhase::BANKRUPTCY); taxDialog.visible=false; return; }
            delete cmd;
            gs.setPhase(GamePhase::PLAYER_TURN);
            taxDialog.visible = false;
        }
    }
}