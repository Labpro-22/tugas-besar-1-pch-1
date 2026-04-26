#include "GameScreen.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Commands/JualBangunanCommand.hpp"

void GameScreen::triggerJualBangunanDialog() {
    jualBangunanDialog.visible = true;
    jualBangunanDialog.scrollY = 0.f;
    jualBangunanDialog.entries.clear();
    jualBangunanDialog.hoveredIdx = -1;

    if (!isRealMode()) return;
    
    GameMaster* gm = guiManager->getGameMaster();
    Player* cur = gm->getState().getCurrPlayer();
    Board* board = gm->getState().getBoard();
    if (!cur || !board) return;

    for (int i = 0; i < board->getSize(); ++i) {
        auto* pt = dynamic_cast<PropertyTile*>(board->getTile(i));
        if (pt && pt->getProperty()) {
            if (auto* sp = dynamic_cast<StreetProperty*>(pt->getProperty())) {
                if (sp->getOwnerId() == cur->getUsername() && (sp->getBuildingCount() > 0 || sp->gethasHotel())) {
                    jualBangunanDialog.entries.push_back({sp, i});
                }
            }
        }
    }
}

void GameScreen::drawJualBangunanDialog() {
    if (!jualBangunanDialog.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,170});

    float PW = 500.f, PH = 400.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22,24,38,255});
    DrawRectangleLinesEx({px, py, PW, PH}, 1.5f, {80,80,130,255});
    DrawRectangle((int)px, (int)py, (int)PW, 44, {32,34,52,255});
    DrawText("JUAL BANGUNAN", (int)(px+16), (int)(py+14), 16, {180,180,230,255});

    Rectangle xBtn = {px+PW-36, py+8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);
    DrawRectangleRec(xBtn, xHov?Color{180,60,60,255}:Color{110,40,40,255});
    DrawText("X", (int)(xBtn.x+9), (int)(xBtn.y+8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov) jualBangunanDialog.visible = false;

    if (jualBangunanDialog.entries.empty()) {
        DrawText("Tidak ada properti dengan bangunan untuk dijual.", (int)px+20, (int)py+80, 14, {150,150,150,255});
        return;
    }

    float listY = py + 60;
    for (size_t i = 0; i < jualBangunanDialog.entries.size(); ++i) {
        float rowY = listY + i * 40;
        if (rowY + 40 > py + PH) break;
        Rectangle row = {px+10, rowY, PW-20, 36};
        bool hover = CheckCollisionPointRec(GetMousePosition(), row);
        DrawRectangleRec(row, hover ? Color{40,45,65,255} : Color{30,32,48,255});
        
        auto* sp = jualBangunanDialog.entries[i].prop;
        std::string status = sp->gethasHotel() ? "HOTEL" : (std::to_string(sp->getBuildingCount()) + " Rumah");
        int sellValue = sp->getHouseUpgCost() / 2;
        std::string txt = sp->getName() + " (" + status + ") - M" + std::to_string(sellValue);
        
        DrawText(txt.c_str(), (int)row.x+10, (int)row.y+10, 14, WHITE);
        
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (isRealMode()) {
                GameMaster* gm = guiManager->getGameMaster();
                GameState&  gs = gm->getState();
                Player* cur    = gs.getCurrPlayer();
                if (cur)
                    guiManager->pushCommand(
                        new JualBangunanCommand(cur, sp, gs.getCurrTurn()));
            }
            jualBangunanDialog.visible = false;
        }
    }
}
