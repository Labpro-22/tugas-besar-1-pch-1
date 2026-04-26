#include "GameScreen.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Commands/GadaiCommand.hpp"

void GameScreen::triggerGadaiDialog() {
    gadaiDialog.visible = true;
    gadaiDialog.scrollY = 0.f;
    gadaiDialog.entries.clear();
    gadaiDialog.hoveredIdx = -1;

    if (!isRealMode()) return;
    
    GameMaster* gm = guiManager->getGameMaster();
    Player* cur = gm->getState().getCurrPlayer();
    Board* board = gm->getState().getBoard();
    if (!cur || !board) return;

    for (int i = 0; i < board->getSize(); ++i) {
        auto* pt = dynamic_cast<PropertyTile*>(board->getTile(i));
        if (pt && pt->getProperty()) {
            Property* p = pt->getProperty();
            if (p->getOwnerId() == cur->getUsername() && p->getStatus() == PropertyStatus::OWNED) {
                bool canGadai = true;
                if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
                    if (sp->getBuildingCount() > 0 || sp->gethasHotel()) canGadai = false;
                }
                if (canGadai) gadaiDialog.entries.push_back({p, i});
            }
        }
    }
}

void GameScreen::drawGadaiDialog() {
    if (!gadaiDialog.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,170});

    float PW = 500.f, PH = 400.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22,24,38,255});
    DrawRectangleLinesEx({px, py, PW, PH}, 1.5f, {80,80,130,255});
    DrawRectangle((int)px, (int)py, (int)PW, 44, {32,34,52,255});
    DrawText("GADAI PROPERTI", (int)(px+16), (int)(py+14), 16, {180,180,230,255});

    Rectangle xBtn = {px+PW-36, py+8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);
    DrawRectangleRec(xBtn, xHov?Color{180,60,60,255}:Color{110,40,40,255});
    DrawText("X", (int)(xBtn.x+9), (int)(xBtn.y+8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov) gadaiDialog.visible = false;

    if (gadaiDialog.entries.empty()) {
        DrawText("Tidak ada properti yang bisa digadaikan.", (int)px+20, (int)py+80, 14, {150,150,150,255});
        return;
    }

    float listY = py + 60;
    for (size_t i = 0; i < gadaiDialog.entries.size(); ++i) {
        float rowY = listY + i * 40;
        if (rowY + 40 > py + PH) break;
        Rectangle row = {px+10, rowY, PW-20, 36};
        bool hover = CheckCollisionPointRec(GetMousePosition(), row);
        DrawRectangleRec(row, hover ? Color{40,45,65,255} : Color{30,32,48,255});
        
        auto* p = gadaiDialog.entries[i].prop;
        std::string txt = p->getName() + " - M" + std::to_string(p->getPurchasePrice() / 2); // Default mock for mortgage
        DrawText(txt.c_str(), (int)row.x+10, (int)row.y+10, 14, WHITE);
        
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (isRealMode()) {
                GameMaster* gm = guiManager->getGameMaster();
                GameState&  gs = gm->getState();
                Player* cur    = gs.getCurrPlayer();
                if (cur)
                    // canGadai sudah terfilter di trigger (tidak ada bangunan)
                    // playerConfirmedSell = true karena sudah pasti tidak ada bangunan
                    guiManager->pushCommand(new GadaiCommand(cur, p, true));
            }
            gadaiDialog.visible = false;
        }
    }
}
