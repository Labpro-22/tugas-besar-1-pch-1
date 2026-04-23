#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/Commands/FestivalCommand.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include <algorithm>
#include <string>

void GameScreen::triggerFestivalDialog()
{
    if (!isRealMode()) return;
    const GameState& gs = guiManager->getGameMaster()->getState();
    Player* curP = gs.getCurrPlayer();
    if (!curP) return;

    festivalDialog.streets.clear();
    for (Property* prop : curP->getProperties()) {
        StreetProperty* sp = dynamic_cast<StreetProperty*>(prop);
        if (sp && sp->getStatus() != PropertyStatus::MORTGAGED)
            festivalDialog.streets.push_back(sp);
    }
    festivalDialog.scrollY    = 0.f;
    festivalDialog.hoveredIdx = -1;
    festivalDialog.visible    = true;
}

void GameScreen::drawFestivalDialog()
{
    if (!festivalDialog.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    constexpr float PW = 500.f, PH = 480.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px,(int)py,(int)PW,(int)PH,{22,24,38,255});
    DrawRectangleLinesEx({px,py,PW,PH},2.f,{239,159,39,255});

    DrawRectangle((int)px,(int)py,(int)PW,52,{180,110,20,255});
    const char* title = "FESTIVAL - Pilih Properti";
    int tw = MeasureText(title,16);
    DrawText(title,(int)(px+PW/2-tw/2),(int)(py+18),16,WHITE);

    // Tombol X
    Rectangle xBtn = {px+PW-36, py+12, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(),xBtn);
    DrawRectangleRec(xBtn, xHov?Color{180,60,60,255}:Color{110,40,40,255});
    DrawText("X",(int)(xBtn.x+9),(int)(xBtn.y+8),12,WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov && isRealMode()) {
        guiManager->getGameMaster()->getState().setPhase(GamePhase::PLAYER_TURN);
        festivalDialog.visible = false;
        return;
    }

    DrawText("Klik properti untuk mengaktifkan festival:",
             (int)(px+16),(int)(py+58),11,{180,180,210,255});

    float listX = px+16, listY = py+76, listW = PW-32;
    float listH = PH - 80 - 56, rowH = 62.f;
    auto& streets = festivalDialog.streets;
    float totalH = streets.size() * rowH;

    Rectangle listArea = {listX,listY,listW,listH};
    if (CheckCollisionPointRec(GetMousePosition(),listArea)) {
        float wheel = GetMouseWheelMove();
        festivalDialog.scrollY -= wheel * 40.f;
        festivalDialog.scrollY = std::max(0.f,
            std::min(festivalDialog.scrollY, std::max(0.f,totalH-listH)));
    }

    Vector2 mouse = GetMousePosition();
    BeginScissorMode((int)listX,(int)listY,(int)listW,(int)listH);
    for (int i = 0; i < (int)streets.size(); i++) {
        StreetProperty* sp = streets[i];
        float ry = listY + i*rowH - festivalDialog.scrollY;
        if (ry+rowH < listY || ry > listY+listH) continue;

        bool hov    = CheckCollisionPointRec(mouse,{listX,ry,listW,rowH-4});
        Color rowBg = hov ? Color{45,50,72,255} : Color{30,32,48,255};
        DrawRectangle((int)listX,(int)ry,(int)listW,(int)(rowH-4),rowBg);
        DrawRectangleLinesEx({listX,ry,listW,rowH-4},1.f,
                             hov?Color{239,159,39,255}:Color{60,60,90,255});

        int mult = sp->getFestivalMultiplier();
        std::string multStr = "x" + std::to_string(mult);
        Color multCol = (mult==1) ? Color{100,100,140,255}
                      : (mult==2) ? Color{239,159, 39,255}
                      : (mult==4) ? Color{186,117, 23,255}
                                  : Color{133, 79, 11,255};
        DrawText(multStr.c_str(),(int)(listX+10),(int)(ry+10),16,multCol);
        DrawText(sp->getCode().c_str(),(int)(listX+50),(int)(ry+8),13,WHITE);
        DrawText(sp->getName().c_str(),(int)(listX+50),(int)(ry+26),11,{180,180,200,255});

        int rent = sp->calculateRentPrice(0,1,false);
        std::string rentStr = "Sewa: M" + std::to_string(rent);
        int rw = MeasureText(rentStr.c_str(),11);
        DrawText(rentStr.c_str(),(int)(listX+listW-rw-12),(int)(ry+8),11,{100,220,100,255});

        if (mult > 1) {
            std::string durStr = "Durasi: "+std::to_string(sp->getFestivalDuration())+" giliran";
            int dw = MeasureText(durStr.c_str(),10);
            DrawText(durStr.c_str(),(int)(listX+listW-dw-12),(int)(ry+26),10,multCol);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hov && isRealMode()) {
            GameMaster* gm = guiManager->getGameMaster();
            GameState&  gs = gm->getState();
            Player* curP   = gs.getCurrPlayer();
            if (curP) {
                FestivalCommand* cmd = new FestivalCommand(curP,gs.getLogger(),gs.getCurrTurn());
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

    if (streets.empty()) {
        const char* msg = "Kamu tidak punya properti lahan yang memenuhi syarat.";
        int mw = MeasureText(msg,11);
        DrawText(msg,(int)(listX+listW/2-mw/2),(int)(listY+listH/2-8),11,{150,150,180,255});
    }

    if (totalH > listH) {
        float sbH = listH*(listH/totalH);
        float sbY = listY+(festivalDialog.scrollY/totalH)*listH;
        DrawRectangle((int)(listX+listW-5),(int)listY,4,(int)listH,{40,42,58,255});
        DrawRectangle((int)(listX+listW-5),(int)sbY,4,(int)sbH,{200,140,40,255});
    }

    DrawText("Klik properti untuk mengaktifkan | X untuk batal",
             (int)(px+16),(int)(py+PH-34),10,{100,100,140,255});
}