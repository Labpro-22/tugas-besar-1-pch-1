#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/Commands/BeliCommand.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include <string>

extern const TileDef TILE_DEFS[40];

void GameScreen::triggerBuyDialog(int tileIdx)
{
    auto& curP = gameState.players[gameState.activePlayerIdx];
    auto& prop = gameState.properties[tileIdx];
    buyDialog.tileIdx   = tileIdx;
    buyDialog.canAfford = (curP.money >= prop.price);
    buyDialog.visible   = true;
}

void GameScreen::drawBuyDialog()
{
    if (!buyDialog.visible || buyDialog.tileIdx < 0) return;

    auto&  prop = gameState.properties[buyDialog.tileIdx];
    auto&  curP = gameState.players[gameState.activePlayerIdx];
    Color  pCol = playerColors[gameState.activePlayerIdx];

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,160});

    float pw = 420, ph = 300;
    float px = SCREEN_W/2.f - pw/2.f;
    float py = SCREEN_H/2.f - ph/2.f;

    DrawRectangle((int)px,(int)py,(int)pw,(int)ph,{25,27,38,255});
    DrawRectangleLinesEx({px,py,pw,ph},2.f,{80,80,130,255});

    Color hdrCol = prop.colorGroup.empty()
                     ? Color{50,52,70,255}
                     : getGroupColor(prop.colorGroup);
    DrawRectangle((int)px,(int)py,(int)pw,52,hdrCol);

    std::string propName = prop.name.empty()
                             ? TILE_DEFS[buyDialog.tileIdx].code
                             : prop.name;
    int nw = MeasureText(propName.c_str(),18);
    DrawText(propName.c_str(),(int)(px+pw/2-nw/2),(int)(py+8),18,WHITE);
    int gw = MeasureText(prop.colorGroup.c_str(),11);
    DrawText(prop.colorGroup.c_str(),(int)(px+pw/2-gw/2),(int)(py+32),11,{255,255,255,180});

    float ry = py + 68;
    DrawText(curP.username.c_str(),(int)(px+20),(int)ry,13,pCol);

    std::string balStr = "Saldo: M" + std::to_string(curP.money);
    int bw = MeasureText(balStr.c_str(),12);
    DrawText(balStr.c_str(),(int)(px+pw-20-bw),(int)ry,12,
             buyDialog.canAfford ? Color{100,220,100,255} : Color{220,80,80,255});
    DrawLine((int)(px+16),(int)(ry+18),(int)(px+pw-16),(int)(ry+18),{60,60,90,255});
    ry += 28;

    DrawText("Harga Beli",(int)(px+20),(int)ry,12,{140,140,180,255});
    std::string priceStr = "M" + std::to_string(prop.price);
    int pw2 = MeasureText(priceStr.c_str(),18);
    DrawText(priceStr.c_str(),(int)(px+pw-20-pw2),(int)(ry-3),18,WHITE);
    ry += 28;

    int remaining = curP.money - prop.price;
    DrawText("Sisa saldo",(int)(px+20),(int)ry,11,{120,120,160,255});
    std::string remStr = "M" + std::to_string(remaining);
    Color remCol = remaining >= 0 ? Color{100,200,100,255} : Color{220,80,80,255};
    int rw = MeasureText(remStr.c_str(),12);
    DrawText(remStr.c_str(),(int)(px+pw-20-rw),(int)ry,12,remCol);
    ry += 26;

    if (!buyDialog.canAfford) {
        const char* warn = "Saldo tidak cukup -- properti akan dilelang";
        int ww = MeasureText(warn,10);
        DrawText(warn,(int)(px+pw/2-ww/2),(int)ry,10,{220,100,80,255});
    }

    float btnY = py + ph - 56;
    float btnW = pw / 2.f - 24;
    Vector2 mouse = GetMousePosition();

    // Tombol BELI
    Rectangle buyBtn = {px+16, btnY, btnW, 40};
    bool buyHover    = CheckCollisionPointRec(mouse, buyBtn) && buyDialog.canAfford;
    bool buyDisabled = !buyDialog.canAfford;
    Color buyBg      = buyDisabled ? Color{40,42,54,255}
                     : buyHover   ? Color{60,180,90,255}
                                  : Color{40,140,70,255};
    DrawRectangleRec(buyBtn, buyBg);
    DrawRectangleLinesEx(buyBtn,1.5f,buyDisabled?Color{60,60,80,255}:Color{80,200,110,255});
    const char* buyLbl = buyDisabled ? "TIDAK MAMPU" : "BELI";
    int blw = MeasureText(buyLbl,13);
    DrawText(buyLbl,(int)(buyBtn.x+btnW/2-blw/2),(int)(btnY+13),13,
             buyDisabled?Color{80,80,100,255}:WHITE);

    // Tombol SKIP
    Rectangle skipBtn = {px+pw/2+8, btnY, btnW, 40};
    bool skipHover    = CheckCollisionPointRec(mouse, skipBtn);
    Color skipBg      = skipHover ? Color{180,100,50,255} : Color{120,60,30,255};
    DrawRectangleRec(skipBtn, skipBg);
    DrawRectangleLinesEx(skipBtn,1.5f,{200,130,80,255});
    const char* skipLbl = "SKIP (LELANG)";
    int slw = MeasureText(skipLbl,11);
    DrawText(skipLbl,(int)(skipBtn.x+btnW/2-slw/2),(int)(btnY+14),11,WHITE);

    // Handle klik
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (buyHover && !buyDisabled) {
            if (guiManager && guiManager->getGameMaster()) {
                GameMaster* gm  = guiManager->getGameMaster();
                Player* player  = gm->getState().getCurrPlayer();
                Board*  board   = gm->getState().getBoard();
                Tile*   tile    = board->getTile(buyDialog.tileIdx); // 0-based
                auto*   pt      = dynamic_cast<PropertyTile*>(tile);
                if (player && pt && pt->getProperty())
                    guiManager->pushCommand(
                        new BeliCommand(player, pt->getProperty(), true));
            } else {
                auto& p = gameState.players[gameState.activePlayerIdx];
                auto& pr = gameState.properties[buyDialog.tileIdx];
                p.money -= pr.price;
                pr.owner = gameState.activePlayerIdx;
                gameState.logger.addLog(gameState.currentTurn, p.username,
                                        "BELI","Beli "+pr.name+": -M"+std::to_string(pr.price));
            }
            buyDialog.visible = false;
        } else if (skipHover) {
            if (guiManager && guiManager->getGameMaster()) {
                GameMaster* gm  = guiManager->getGameMaster();
                Player* player  = gm->getState().getCurrPlayer();
                Board*  board   = gm->getState().getBoard();
                Tile*   tile    = board->getTile(buyDialog.tileIdx); // 0-based
                auto*   pt      = dynamic_cast<PropertyTile*>(tile);
                if (player && pt && pt->getProperty())
                    guiManager->pushCommand(
                        new BeliCommand(player, pt->getProperty(), false));
            } else {
                auto& p  = gameState.players[gameState.activePlayerIdx];
                auto& pr = gameState.properties[buyDialog.tileIdx];
                gameState.logger.addLog(gameState.currentTurn, p.username,
                                        "BELI","Skip "+pr.name+" -> lelang");
            }
            buyDialog.visible = false;
        }
    }
}