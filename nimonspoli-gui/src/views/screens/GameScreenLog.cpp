#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/utils/TransactionLogger.hpp"
#include <algorithm>
#include <string>

void GameScreen::initMockLogs()
{
    gameState.logger.addLog(15,"Uname1","DADU","Lempar: 4+5=9, mendarat di BDG");
    gameState.logger.addLog(15,"Uname1","SEWA","Bayar sewa ke Uname2: -M 260");
    gameState.logger.addLog(14,"Uname2","BANGUN","Bangun rumah di BDG: -M 200");
    gameState.logger.addLog(14,"Uname3","KARTU","Dapat Kesempatan: maju 3 petak");
    gameState.logger.addLog(14,"Uname3","DADU","Lempar: 2+1=3, mendarat di FES");
    gameState.logger.addLog(13,"Uname4","GADAI","Gadai MGL: +M 100");
    gameState.logger.addLog(13,"Uname1","BELI","Beli JKT: -M 350");
    gameState.logger.addLog(13,"Uname2","DADU","Lempar: 6+6=12 (double!), mendarat di BDG");
    gameState.logger.addLog(12,"Uname3","BAYAR","Bayar PPH flat: -M 150");
    gameState.logger.addLog(12,"Uname4","LELANG","Menang lelang GRT: M 45");
    gameState.logger.addLog(12,"Uname1","DADU","Lempar: 3+4=7, mendarat di GBR");
    gameState.logger.addLog(11,"Uname2","KARTU","Dapat Dana Umum: +M 100");
    gameState.logger.addLog(11,"Uname3","BANGUN","Bangun rumah di MKS: -M 150");
    gameState.logger.addLog(11,"Uname4","DADU","Lempar: 1+2=3, mendarat di TSK");
    gameState.logger.addLog(10,"Uname1","SEWA","Bayar sewa ke Uname3: -M 700");
}

std::string GameScreen::getActionIcon(const std::string& action)
{
    if (action == "DADU")   return "[D]";
    if (action == "BELI")   return "[B]";
    if (action == "SEWA")   return "[$]";
    if (action == "BANGUN") return "[H]";
    if (action == "GADAI")  return "[G]";
    if (action == "KARTU")  return "[K]";
    if (action == "LELANG") return "[L]";
    if (action == "BAYAR")  return "[P]";
    if (action == "FESTIVAL") return "[F]";
    if (action == "BANKRUPT") return "[X]";
    return "[?]";
}

Color GameScreen::getActionColor(const std::string& action)
{
    if (action == "DADU")   return {100,180,240,255};
    if (action == "BELI")   return {100,200,120,255};
    if (action == "SEWA")   return {220,100,100,255};
    if (action == "BANGUN") return {100,220,120,255};
    if (action == "GADAI")  return {220,160, 60,255};
    if (action == "KARTU")  return {180,120,220,255};
    if (action == "LELANG") return {240,200, 60,255};
    if (action == "BAYAR")  return {220, 80, 80,255};
    if (action == "FESTIVAL") return {255, 100, 200, 255};
    if (action == "BANKRUPT") return {255, 50, 50, 255};
    return {180,180,180,255};
}

void GameScreen::drawLogPopup()
{
    if (!showLogPopup) return;

    TransactionLogger* logSrc = &gameState.logger;
    if (isRealMode()) {
        TransactionLogger* realLogger =
            guiManager->getGameMaster()->getState().getLogger();
        if (realLogger) logSrc = realLogger;
    }

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    float pw = 700, ph = 540;
    float px = SCREEN_W/2.f - pw/2.f;
    float py = SCREEN_H/2.f - ph/2.f;

    DrawRectangle((int)px,(int)py,(int)pw,(int)ph,{25,27,38,255});
    DrawRectangleLinesEx({px,py,pw,ph},1.5f,{80,80,120,255});
    DrawRectangle((int)px,(int)py,(int)pw,44,{35,37,52,255});
    DrawRectangleLinesEx({px,py,pw,44},1,{80,80,120,255});
    DrawText("LOG TRANSAKSI",(int)(px+16),(int)(py+14),15,{180,180,220,255});

    // N input box
    float ibX=px+pw-180, ibY=py+10, ibW=100, ibH=28;
    Rectangle inputBox={ibX,ibY,ibW,ibH};
    bool hover = CheckCollisionPointRec(GetMousePosition(),inputBox);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) logNFocused = hover;

    DrawRectangleRec(inputBox,logNFocused?Color{40,45,65,255}:Color{30,32,48,255});
    DrawRectangleLinesEx(inputBox,1,logNFocused?Color{120,160,240,255}:Color{70,70,100,255});
    DrawText("Tampilkan:",(int)(ibX-MeasureText("Tampilkan:",11)-8),(int)(ibY+8),11,{140,140,180,255});

    if (logNFocused) {
        if (IsKeyPressed(KEY_BACKSPACE) && !logNInput.empty()) logNInput.pop_back();
        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch>='0' && ch<='9' && logNInput.size()<3) logNInput += (char)ch;
            ch = GetCharPressed();
        }
        if (IsKeyPressed(KEY_ENTER)) {
            logShowN = logNInput.empty() ? 0 : std::stoi(logNInput);
            logNFocused = false;
        }
    }

    std::string inputDisplay = logNInput.empty() ? "Semua" : logNInput;
    if (logNInput.empty() && logNFocused) inputDisplay = "";
    int tw = MeasureText(inputDisplay.c_str(),12);
    DrawText(inputDisplay.c_str(),(int)(ibX+ibW/2-tw/2),(int)(ibY+7),12,
             logNInput.empty()?Color{100,100,140,255}:WHITE);
    if (logNFocused && (int)(GetTime()*2)%2==0)
        DrawText("|",(int)(ibX+ibW/2+tw/2+2),(int)(ibY+7),12,WHITE);

    // Close button
    Rectangle closeBtn={px+pw-36,py+8,28,28};
    bool closeHover=CheckCollisionPointRec(GetMousePosition(),closeBtn);
    DrawRectangleRec(closeBtn,closeHover?Color{180,60,60,255}:Color{120,40,40,255});
    DrawRectangleLinesEx(closeBtn,1,{200,80,80,255});
    DrawText("X",(int)(closeBtn.x+9),(int)(closeBtn.y+8),12,WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && closeHover) showLogPopup = false;

    // Column headers
    float headerY = py + 50;
    DrawRectangle((int)px,(int)headerY,(int)pw,24,{35,37,52,255});
    DrawText("TURN",(int)(px+16),(int)(headerY+6),11,{120,120,160,255});
    DrawText("AKSI",(int)(px+80),(int)(headerY+6),11,{120,120,160,255});
    DrawText("PEMAIN",(int)(px+150),(int)(headerY+6),11,{120,120,160,255});
    DrawText("DETAIL",(int)(px+270),(int)(headerY+6),11,{120,120,160,255});
    DrawLine((int)px,(int)(headerY+24),(int)(px+pw),(int)(headerY+24),{60,60,90,255});

    // Logs
    auto all = (logShowN==0) ? logSrc->getLogs() : logSrc->getLastLogs(logShowN);
    std::vector<LogEntry> display(all.rbegin(),all.rend());

    float listY1 = headerY + 26;
    float listH  = ph - (listY1-py) - 10;
    float rowH   = 32.f;
    float totalH = display.size() * rowH;

    Rectangle listArea={px,listY1,pw,listH};
    if (CheckCollisionPointRec(GetMousePosition(),listArea)) {
        float wheel = GetMouseWheelMove();
        logScrollY -= wheel*40.f;
        logScrollY = std::max(0.f,std::min(logScrollY,std::max(0.f,totalH-listH)));
    }

    BeginScissorMode((int)px,(int)listY1,(int)pw,(int)listH);
    for (int i = 0; i < (int)display.size(); i++) {
        auto& e = display[i];
        float ry = listY1 + i*rowH - logScrollY;
        if (ry+rowH < listY1 || ry > listY1+listH) continue;

        Color rowBg = (i%2==0)?Color{28,30,42,255}:Color{32,34,48,255};
        DrawRectangle((int)px,(int)ry,(int)pw,(int)rowH-1,rowBg);
        DrawText(("T"+std::to_string(e.turn)).c_str(),(int)(px+16),(int)(ry+9),11,{120,120,160,255});

        Color ac = getActionColor(e.actionType);
        std::string icon = getActionIcon(e.actionType);
        DrawRectangle((int)(px+70),(int)(ry+6),60,20,{ac.r,ac.g,ac.b,50});
        DrawRectangleLinesEx({px+70,ry+6,60,20},1,{ac.r,ac.g,ac.b,150});
        int iw = MeasureText(icon.c_str(),10);
        DrawText(icon.c_str(),(int)(px+100-iw/2),(int)(ry+9),10,ac);

        Color uc = {180,180,180,255};
        for (int p = 0; p < (int)gameState.players.size(); p++)
            if (gameState.players[p].username == e.username) { uc = playerColors[p]; break; }
        DrawText(e.username.c_str(),(int)(px+150),(int)(ry+9),11,uc);
        DrawText(e.detail.c_str(),(int)(px+270),(int)(ry+9),11,{200,200,215,255});
    }
    EndScissorMode();

    if (totalH > listH) {
        float sbH = listH*(listH/totalH);
        float sbY = listY1+(logScrollY/totalH)*listH;
        DrawRectangle((int)(px+pw-6),(int)listY1,4,(int)listH,{40,42,58,255});
        DrawRectangle((int)(px+pw-6),(int)sbY,4,(int)sbH,{100,100,160,255});
    }

    if (display.empty()) {
        int tw2 = MeasureText("Belum ada log.",14);
        DrawText("Belum ada log.",(int)(px+pw/2-tw2/2),(int)(listY1+listH/2-10),14,{100,100,140,255});
    }
}