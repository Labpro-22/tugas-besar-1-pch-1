#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Player/Player.hpp"
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  CardDialog
// ─────────────────────────────────────────────────────────────────────────────

void GameScreen::triggerCardDialog()
{
    if (!isRealMode()) return;
    const GameState& gs = guiManager->getGameMaster()->getState();
    cardDialog.deckLabel   = gs.getPendingCardDeck();
    cardDialog.description = gs.getPendingCardDesc();
    cardDialog.visible     = true;
}

void GameScreen::drawCardDialog()
{
    if (!cardDialog.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    constexpr float PW = 440.f, PH = 260.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    bool  isKSP  = (cardDialog.deckLabel == "Kesempatan");
    Color hdrCol = isKSP ? Color{180,120,  0,255} : Color{ 60, 60,200,255};
    Color bordCol= isKSP ? Color{240,180, 40,255} : Color{100,120,240,255};
    Color iconCol= isKSP ? Color{255,200, 60,255} : Color{140,160,255,255};

    DrawRectangle((int)px,(int)py,(int)PW,(int)PH,{22,24,38,255});
    DrawRectangleLinesEx({px,py,PW,PH},2.f,bordCol);
    DrawRectangle((int)px,(int)py,(int)PW,52,hdrCol);

    const char* icon = isKSP ? "?" : "$";
    DrawText(icon,(int)(px+20),(int)(py+14),22,iconCol);
    std::string dl = cardDialog.deckLabel;
    int dlw = MeasureText(dl.c_str(),17);
    DrawText(dl.c_str(),(int)(px+PW/2-dlw/2),(int)(py+17),17,WHITE);

    DrawText(cardDialog.description.c_str(),(int)(px+24),(int)(py+70),13,{220,220,240,255});
    DrawLine((int)(px+16),(int)(py+PH-64),(int)(px+PW-16),(int)(py+PH-64),{60,60,90,255});

    float btnW = 120.f;
    Rectangle okBtn = {px+PW/2-btnW/2, py+PH-52, btnW, 38};
    bool okHov = CheckCollisionPointRec(GetMousePosition(),okBtn);
    DrawRectangleRec(okBtn,okHov?Color{80,160,100,255}:Color{50,110,70,255});
    DrawRectangleLinesEx(okBtn,1.5f,{100,200,130,255});
    int okw = MeasureText("OK",14);
    DrawText("OK",(int)(okBtn.x+btnW/2-okw/2),(int)(okBtn.y+11),14,WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && okHov) {
        if (isRealMode())
            guiManager->getGameMaster()->getState().setPhase(GamePhase::PLAYER_TURN);
        cardDialog.visible = false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  JailDialog
// ─────────────────────────────────────────────────────────────────────────────

void GameScreen::triggerJailDialog()
{
    if (!isRealMode()) return;
    const GameState& gs = guiManager->getGameMaster()->getState();
    Player* curP = gs.getCurrPlayer();
    if (!curP) return;

    jailDialog.jailFine      = 50;  // default fine — sesuaikan dengan config jika ada
    jailDialog.canAffordFine = curP->canAfford(jailDialog.jailFine);
    jailDialog.jailTurnsLeft = curP->getJailTurns();
    jailDialog.visible       = true;
}

void GameScreen::drawJailDialog()
{
    if (!jailDialog.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    constexpr float PW = 420.f, PH = 280.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px,(int)py,(int)PW,(int)PH,{22,24,38,255});
    DrawRectangleLinesEx({px,py,PW,PH},2.f,{160,80,80,255});
    DrawRectangle((int)px,(int)py,(int)PW,52,{160,50,50,255});

    const char* title = "KAMU DI PENJARA!";
    int tw = MeasureText(title,17);
    DrawText(title,(int)(px+PW/2-tw/2),(int)(py+17),17,WHITE);

    float ry = py + 68;
    std::string turnsStr = "Sisa giliran penjara: " + std::to_string(jailDialog.jailTurnsLeft);
    DrawText(turnsStr.c_str(),(int)(px+20),(int)ry,12,{200,200,210,255});
    ry += 24;

    std::string fineStr = "Denda keluar: M" + std::to_string(jailDialog.jailFine);
    DrawText(fineStr.c_str(),(int)(px+20),(int)ry,12,{220,200,100,255});
    ry += 30;

    DrawLine((int)(px+16),(int)ry,(int)(px+PW-16),(int)ry,{60,60,90,255});
    ry += 16;
    DrawText("Pilih aksi:",(int)(px+20),(int)ry,12,{180,180,210,255});

    float btnY = py + PH - 60;
    float btnW = (PW - 48) / 3.f;
    Vector2 mouse = GetMousePosition();

    // Tombol BAYAR DENDA
    Rectangle payBtn = {px+16, btnY, btnW, 44};
    bool payHov = CheckCollisionPointRec(mouse,payBtn) && jailDialog.canAffordFine;
    bool payDis = !jailDialog.canAffordFine;
    DrawRectangleRec(payBtn, payDis?Color{40,42,54,255}:payHov?Color{60,180,90,255}:Color{40,130,70,255});
    DrawRectangleLinesEx(payBtn,1.5f,payDis?Color{60,60,80,255}:Color{80,200,110,255});
    const char* payLbl = payDis ? "TIDAK MAMPU" : "BAYAR DENDA";
    int plw = MeasureText(payLbl,10);
    DrawText(payLbl,(int)(payBtn.x+btnW/2-plw/2),(int)(btnY+16),10,
             payDis?Color{80,80,100,255}:WHITE);

    // Tombol PAKAI KARTU
    Rectangle cardBtn = {px+16+btnW+8, btnY, btnW, 44};
    bool cardHov = CheckCollisionPointRec(mouse,cardBtn);
    DrawRectangleRec(cardBtn,cardHov?Color{60,110,200,255}:Color{40,80,160,255});
    DrawRectangleLinesEx(cardBtn,1.5f,{100,150,240,255});
    const char* cardLbl = "PAKAI KARTU";
    int clw = MeasureText(cardLbl,10);
    DrawText(cardLbl,(int)(cardBtn.x+btnW/2-clw/2),(int)(btnY+16),10,WHITE);

    // Tombol TUNGGU
    Rectangle waitBtn = {px+16+2*(btnW+8), btnY, btnW, 44};
    bool waitHov = CheckCollisionPointRec(mouse,waitBtn);
    DrawRectangleRec(waitBtn,waitHov?Color{100,80,60,255}:Color{70,55,40,255});
    DrawRectangleLinesEx(waitBtn,1.5f,{160,120,80,255});
    const char* waitLbl = "TUNGGU";
    int wlw = MeasureText(waitLbl,10);
    DrawText(waitLbl,(int)(waitBtn.x+btnW/2-wlw/2),(int)(btnY+16),10,WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (payHov && !payDis) {
            // TODO: push JailCommand(PAY)
            jailDialog.visible = false;
        } else if (cardHov) {
            // TODO: push JailCommand(USE_CARD)
            jailDialog.visible = false;
        } else if (waitHov) {
            jailDialog.visible = false;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  PropertiPopup — daftar properti milik pemain aktif
// ─────────────────────────────────────────────────────────────────────────────

void GameScreen::drawPropertiPopup()
{
    if (!propertiPopup.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    constexpr float PW = 520.f, PH = 500.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px,(int)py,(int)PW,(int)PH,{22,24,38,255});
    DrawRectangleLinesEx({px,py,PW,PH},2.f,{80,80,130,255});
    DrawRectangle((int)px,(int)py,(int)PW,44,{32,34,52,255});
    DrawRectangleLinesEx({px,py,PW,44},1,{80,80,130,255});
    DrawText("DAFTAR PROPERTI",(int)(px+16),(int)(py+14),14,{180,180,230,255});

    // Tombol X
    Rectangle xBtn = {px+PW-36,py+8,28,28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(),xBtn);
    DrawRectangleRec(xBtn,xHov?Color{180,60,60,255}:Color{110,40,40,255});
    DrawText("X",(int)(xBtn.x+9),(int)(xBtn.y+8),12,WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
        propertiPopup.visible = false;

    // Daftar properti milik pemain aktif
    float listX = px+16, listY = py+52;
    float listW = PW-32, listH = PH-60;
    float rowH  = 54.f;

    std::vector<int> owned;
    for (int i = 0; i < (int)gameState.properties.size(); i++)
        if (gameState.properties[i].owner == gameState.activePlayerIdx)
            owned.push_back(i);
    float totalH = owned.size() * rowH;

    Rectangle listArea = {listX,listY,listW,listH};
    if (CheckCollisionPointRec(GetMousePosition(),listArea)) {
        float wheel = GetMouseWheelMove();
        propertiPopup.scrollY -= wheel * 40.f;
        propertiPopup.scrollY = std::max(0.f,
            std::min(propertiPopup.scrollY, std::max(0.f,totalH-listH)));
    }

    BeginScissorMode((int)listX,(int)listY,(int)listW,(int)listH);
    Color pCol = playerColors[gameState.activePlayerIdx];
    for (int ii = 0; ii < (int)owned.size(); ii++) {
        int i = owned[ii];
        auto& prop = gameState.properties[i];
        float ry   = listY + ii*rowH - propertiPopup.scrollY;
        if (ry+rowH < listY || ry > listY+listH) continue;

        Color rowBg = {30,32,48,255};
        DrawRectangle((int)listX,(int)ry,(int)listW,(int)(rowH-4),rowBg);
        DrawRectangleLinesEx({listX,ry,listW,rowH-4},1.f,{60,60,90,255});

        // Color swatch
        Color grpCol = prop.colorGroup.empty() ? GRAY : getGroupColor(prop.colorGroup);
        DrawRectangle((int)listX,(int)ry,8,(int)(rowH-4),grpCol);

        std::string pname = prop.name.empty() ? prop.code : prop.name;
        DrawText(pname.c_str(),(int)(listX+16),(int)(ry+8),13,WHITE);

        // Buildings
        std::string bldStr = prop.buildings == 5 ? "Hotel" :
                             prop.buildings  > 0 ? std::to_string(prop.buildings)+" rumah" : "-";
        DrawText(bldStr.c_str(),(int)(listX+16),(int)(ry+28),11,pCol);

        // Mortgaged
        if (prop.mortgaged) DrawText("GADAI",(int)(listX+listW-80),(int)(ry+8),11,RED);

        // Sewa
        std::string rentStr = "Sewa: M"+std::to_string(prop.rentL0);
        int rw = MeasureText(rentStr.c_str(),10);
        DrawText(rentStr.c_str(),(int)(listX+listW-rw-8),(int)(ry+28),10,{100,220,100,255});
    }
    EndScissorMode();

    if (owned.empty()) {
        const char* msg = "Belum memiliki properti.";
        int mw = MeasureText(msg,13);
        DrawText(msg,(int)(listX+listW/2-mw/2),(int)(listY+listH/2-8),13,{130,130,160,255});
    }

    if (totalH > listH) {
        float sbH = listH*(listH/totalH);
        float sbY = listY+(propertiPopup.scrollY/totalH)*listH;
        DrawRectangle((int)(listX+listW-5),(int)listY,4,(int)listH,{40,42,58,255});
        DrawRectangle((int)(listX+listW-5),(int)sbY,4,(int)sbH,{120,120,200,255});
    }
}