#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/Commands/LemparDaduCommand.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include <string>
#include <cmath>

extern const TileDef TILE_DEFS[40];

// ─── Internal helper (same as in Board) ──────────────────────────────────────
static void DrawRoundedBorder_Dice(Rectangle rec, float roundness, int segments,
                                   float thick, Color color)
{
    for (float i = 0.f; i < thick; i += 0.5f) {
        Rectangle r = {rec.x-i, rec.y-i, rec.width+i*2, rec.height+i*2};
        DrawRectangleRoundedLines(r, roundness, segments, color);
    }
}

// ─── Draw dice area ───────────────────────────────────────────────────────────
void GameScreen::drawDiceArea()
{
    float innerX = boardX + CORNER_SZ;
    float innerY = boardY + CORNER_SZ;
    float innerSz = 9 * TILE_W;
    float cx = innerX + innerSz / 2.f;
    float cy = innerY + innerSz / 2.f;

    constexpr float DIE_SZ = 54.f;
    constexpr float GAP    = 16.f;
    float totalW = DIE_SZ * 2 + GAP;
    float d1x = cx - totalW / 2.f;
    float d2x = d1x + DIE_SZ + GAP;
    float dy  = cy - DIE_SZ / 2.f;

    int disp1 = diceState.val1;
    int disp2 = diceState.val2;
    if (diceState.animating) {
        float t = diceState.animTimer / DiceState::ANIM_DURATION;
        if (t < 0.85f) {
            disp1 = GetRandomValue(1, 6);
            disp2 = GetRandomValue(1, 6);
        }
    }

    bool showResult = diceState.val1 > 0;

    auto drawDie = [&](float x, float y, int val, bool highlight) {
        Color bg     = highlight ? Color{255,230,80,255} : Color{240,240,240,255};
        Color border = highlight ? Color{200,160, 0,255} : Color{160,160,160,255};
        Color dotCol = {30,30,30,255};

        if (diceState.animating) {
            float shake = 3.f * (1.f - diceState.animTimer / DiceState::ANIM_DURATION);
            x += (float)GetRandomValue(-1,1) * shake;
            y += (float)GetRandomValue(-1,1) * shake;
        }

        DrawRectangleRounded({x,y,DIE_SZ,DIE_SZ}, 0.2f, 8, bg);
        DrawRoundedBorder_Dice({x,y,DIE_SZ,DIE_SZ}, 0.2f, 8, 2.f, border);

        if (!showResult && !diceState.animating) {
            int qw = MeasureText("?", 24);
            DrawText("?", (int)(x+DIE_SZ/2-qw/2), (int)(y+DIE_SZ/2-12), 24, {150,150,150,255});
            return;
        }

        class Dot {
        public:
            float nx, ny;
        };
        static const Dot dots[6][6] = {
            {{.5f,.5f}},
            {{.25f,.25f},{.75f,.75f}},
            {{.25f,.25f},{.5f,.5f},{.75f,.75f}},
            {{.25f,.25f},{.75f,.25f},{.25f,.75f},{.75f,.75f}},
            {{.25f,.25f},{.75f,.25f},{.5f,.5f},{.25f,.75f},{.75f,.75f}},
            {{.25f,.25f},{.75f,.25f},{.25f,.5f},{.75f,.5f},{.25f,.75f},{.75f,.75f}},
        };
        static const int dotCount[6] = {1,2,3,4,5,6};

        int v = (val >= 1 && val <= 6) ? val : 1;
        for (int d = 0; d < dotCount[v-1]; d++) {
            float dotX = x + dots[v-1][d].nx * DIE_SZ;
            float dotY = y + dots[v-1][d].ny * DIE_SZ;
            DrawCircle((int)dotX, (int)dotY, 5.f, dotCol);
        }
    };

    bool dbl = diceState.isDouble && !diceState.animating && showResult;
    drawDie(d1x, dy, disp1, dbl);
    drawDie(d2x, dy, disp2, dbl);

    if (showResult && !diceState.animating) {
        int total = diceState.val1 + diceState.val2;
        std::string lbl = std::to_string(diceState.val1) + " + " +
                          std::to_string(diceState.val2) + " = " +
                          std::to_string(total);
        int lw = MeasureText(lbl.c_str(), 13);
        DrawText(lbl.c_str(), (int)(cx-lw/2), (int)(dy+DIE_SZ+8), 13,
                 diceState.isDouble ? Color{255,220,50,255} : Color{200,200,210,255});
        if (diceState.isDouble) {
            const char* dblLbl = "DOUBLE!";
            int dw = MeasureText(dblLbl, 12);
            DrawText(dblLbl, (int)(cx-dw/2), (int)(dy+DIE_SZ+26), 12, {255,200,50,255});
        }
    }
}

// ─── Handle lempar dadu ───────────────────────────────────────────────────────
void GameScreen::handleLemparDadu()
{
    if (guiManager && guiManager->getGameMaster()) {
        GameMaster*      gm    = guiManager->getGameMaster();
        const GameState& gs    = gm->getState();

        // Blok roll jika ada aksi pending (beli, pajak, festival, lelang, dll)
        GamePhase phase = gs.getPhase();
        if (phase != GamePhase::PLAYER_TURN) return;

        // Blok jika sudah roll (termasuk saat extra turn menunggu aksi selesai)
        if (gs.getHasRolled()) return;

        Player* player = gs.getCurrPlayer();
        Dice*   dice   = gs.getDice();
        if (player && dice)
            guiManager->pushCommand(new LemparDaduCommand(*gm, player, *dice));
        return;
    }

    // ── Mode mock ────────────────────────────────────────────────────────
    if (diceState.hasRolled || diceState.animating) return;

    diceState.val1      = 4;
    diceState.val2      = 5;
    int total           = diceState.val1 + diceState.val2;
    diceState.isDouble  = (diceState.val1 == diceState.val2);
    diceState.tripleDouble = false;
    diceState.hasRolled = true;
    diceState.animating = true;
    diceState.animTimer = 0.f;

    auto& curP  = gameState.players[gameState.activePlayerIdx];
    int newPos  = (curP.position + total) % 40;

    std::string detail = "Lempar: " + std::to_string(diceState.val1) + "+" +
                         std::to_string(diceState.val2) + "=" + std::to_string(total) +
                         ", mendarat di " + TILE_DEFS[newPos].code;
    if (diceState.isDouble) detail += " (double!)";
    gameState.logger.addLog(gameState.currentTurn, curP.username, "DADU", detail);
    curP.position = newPos;

    auto& landedProp = gameState.properties[newPos];
    if (landedProp.type == "STREET" && landedProp.owner == -1)
        triggerBuyDialog(newPos);
}