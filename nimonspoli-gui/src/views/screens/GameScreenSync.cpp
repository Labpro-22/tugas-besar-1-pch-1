#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  syncFromGameMaster()
//
//  Dipanggil setiap frame di update() jika isRealMode() == true.
//  Mengisi ulang MockGameState dari data real sehingga semua fungsi render
//  (drawBoard, drawLeftPanel, dll.) tidak perlu diubah.
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::syncFromGameMaster()
{
    if (!isRealMode())
        return;

    GameMaster*       gm    = guiManager->getGameMaster();
    const GameState&  gs    = gm->getState();
    Board*            board = gs.getBoard();

    gameState.currentTurn     = gs.getCurrTurn();
    gameState.maxTurn         = gs.getMaxTurn();
    gameState.activePlayerIdx = gs.getCurrPlayerIdx();

    // ── Sync Players ──────────────────────────────────────────────────────
    const auto& realPlayers = gs.getPlayers();
    if (playerVisuals.size() != realPlayers.size()) {
        playerVisuals.resize(realPlayers.size());
        for (int i = 0; i < (int)realPlayers.size(); i++) {
            playerVisuals[i].currentTileIdx = (float)(realPlayers[i]->getPosition());
            playerVisuals[i].targetTileIdx  = playerVisuals[i].currentTileIdx;
        }
    }
    
    gameState.players.clear();
    gameState.players.resize(realPlayers.size());

    for (int i = 0; i < (int)realPlayers.size(); ++i) {
        Player*      p  = realPlayers[i];
        MockPlayer&  mp = gameState.players[i];

        mp.username  = p->getUsername();
        mp.money     = p->getBalance();
        int realPos = p->getPosition();
        mp.position  = realPos;
        mp.cardCount = p->getHandSize();
        mp.status    = (p->getStatus() == PlayerStatus::JAILED)   ? "JAILED"
                     : (p->getStatus() == PlayerStatus::BANKRUPT) ? "BANKRUPT"
                                                                   : "ACTIVE";

        // Logika animasi: jika posisi real berubah, set target baru
        if (realPos != (int)playerVisuals[i].targetTileIdx)
            playerVisuals[i].targetTileIdx = (float)realPos;
    }

    // ── Sync Properties ───────────────────────────────────────────────────
    if (board) {
        for (int i = 0; i < (int)gameState.properties.size(); ++i) {
            Tile* tile = board->getTile(i);  // indeks core 1-based
            if (!tile) continue;

            PropertyTile* pt = dynamic_cast<PropertyTile*>(tile);
            if (!pt) continue;

            Property*    prop = pt->getProperty();
            MockProperty& mp  = gameState.properties[i];

            const std::string& ownerId = prop->getOwnerId();
            mp.owner    = -1;
            mp.mortgaged = (prop->getStatus() == PropertyStatus::MORTGAGED);

            if (prop->getStatus() != PropertyStatus::BANK) {
                for (int pi = 0; pi < (int)realPlayers.size(); ++pi) {
                    if (realPlayers[pi]->getUsername() == ownerId) {
                        mp.owner = pi;
                        break;
                    }
                }
            }
            mp.price = prop->getPurchasePrice();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  syncDiceResult()
//
//  Dipanggil dari GUIManager::run() SETELAH flushCommands(), karena
//  LemparDaduCommand baru mengubah nilai Dice di dalam execute().
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::syncDiceResult()
{
    if (!isRealMode())
        return;

    GameMaster* gm   = guiManager->getGameMaster();
    Dice*       dice = gm->getState().getDice();
    if (!dice) return;

    int v1 = dice->getDaduVal1();
    int v2 = dice->getDaduVal2();

    bool justRolled = gm->getState().getHasRolled() && !diceState.hasRolled;
    bool valChanged = (v1 != diceState.val1 || v2 != diceState.val2);

    if ((valChanged || justRolled) && v1 > 0) {
        diceState.val1      = v1;
        diceState.val2      = v2;
        diceState.animating = true;
        diceState.animTimer = 0.f;
    }
    diceState.hasRolled = gm->getState().getHasRolled();

    // ── Trigger Dialogs ────────────────────────────────────────────────────
    const GameState& gs = gm->getState();
    if (gs.getPhase() == GamePhase::AWAITING_BUY && !buyDialog.visible)
        triggerBuyDialog(gs.getCurrPlayer()->getPosition());
    if (gs.getPhase() == GamePhase::AWAITING_TAX && !taxDialog.visible)
        triggerTaxDialog();
    if (gs.getPhase() == GamePhase::AWAITING_FESTIVAL && !festivalDialog.visible)
        triggerFestivalDialog();
    if (gs.getPhase() == GamePhase::SHOW_CARD && !cardDialog.visible)
        triggerCardDialog();
}