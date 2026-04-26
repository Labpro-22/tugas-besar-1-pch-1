#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/RailroadProperty.hpp"
#include "../../core/Property/UtilityProperty.hpp"
#include "../../core/Player/Player.hpp"

void GameScreen::syncFromGameMaster()
{
    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Board *board = gs.getBoard();

    gameState.currentTurn = gs.getCurrTurn();
    gameState.maxTurn = gs.getMaxTurn();
    gameState.activePlayerIdx = gs.getCurrPlayerIdx();

    if (gs.getPhase() != GamePhase::GAME_OVER)
    {
        if (gs.isMaxTurnReached() || gs.countActivePlayers() <= 1)
            gs.setPhase(GamePhase::GAME_OVER);
    }
    if (gs.getPhase() == GamePhase::GAME_OVER)
    {
        gameOver = true;
        return;
    }

    // ── Sync Players ──────────────────────────────────────────────────────
    const auto &realPlayers = gs.getPlayers();
    if (playerVisuals.size() != realPlayers.size())
    {
        playerVisuals.resize(realPlayers.size());
        for (int i = 0; i < (int)realPlayers.size(); i++)
        {
            playerVisuals[i].currentTileIdx = (float)realPlayers[i]->getPosition();
            playerVisuals[i].targetTileIdx = playerVisuals[i].currentTileIdx;
        }
    }

    gameState.players.clear();
    gameState.players.resize(realPlayers.size());

    for (int i = 0; i < (int)realPlayers.size(); ++i)
    {
        Player *p = realPlayers[i];
        MockPlayer &mp = gameState.players[i];

        mp.username = p->getUsername();
        mp.money = p->getBalance();
        int realPos = p->getPosition();
        mp.position = realPos;
        mp.cardCount = p->getHandSize();
        mp.status = (p->getStatus() == PlayerStatus::JAILED)     ? "JAILED"
                    : (p->getStatus() == PlayerStatus::BANKRUPT) ? "BANKRUPT"
                                                                 : "ACTIVE";
        if (realPos != (int)playerVisuals[i].targetTileIdx)
            playerVisuals[i].targetTileIdx = (float)realPos;
    }

    // ── Sync Properties ───────────────────────────────────────────────────
    if (board)
    {
        for (int i = 0; i < (int)gameState.properties.size(); ++i)
        {
            Tile *tile = board->getTile(i);
            if (!tile)
                continue;
            PropertyTile *pt = dynamic_cast<PropertyTile *>(tile);
            if (!pt)
                continue;
            Property *prop = pt->getProperty();
            if (!prop)
                continue;

            MockProperty &mp = gameState.properties[i];
            mp.name = prop->getName();
            mp.colorGroup = prop->getColorGroup();
            mp.owner = -1;
            mp.mortgaged = (prop->getStatus() == PropertyStatus::MORTGAGED);

            if (prop->getStatus() != PropertyStatus::BANK)
            {
                for (int pi = 0; pi < (int)realPlayers.size(); ++pi)
                {
                    if (realPlayers[pi]->getUsername() == prop->getOwnerId())
                    {
                        mp.owner = pi;
                        break;
                    }
                }
            }
            mp.price = prop->getPurchasePrice();

            if (auto *sp = dynamic_cast<StreetProperty *>(prop))
            {
                mp.type = "STREET";
                mp.buildings = sp->gethasHotel() ? 5 : sp->getBuildingCount();
                mp.festivalMult = sp->getFestivalMultiplier();
                mp.festivalDur = sp->getFestivalDuration();
            }
            else
            {
                mp.type = dynamic_cast<RailroadProperty *>(prop)  ? "RAILROAD"
                          : dynamic_cast<UtilityProperty *>(prop) ? "UTILITY"
                                                                  : "ACTION";
                mp.buildings = 0;
                mp.festivalMult = 1;
                mp.festivalDur = 0;
            }
        }
    }

    // ── Trigger Dialogs ───────────────────────────────────────────────────
    GamePhase phase = gs.getPhase();
    Player *currP = gs.getCurrPlayer();
    
    bool blockTileDialogs = skillCardResultDialog.visible && pendingTileEffectAfterCard;

    if (!blockTileDialogs)
    {
        if (phase == GamePhase::AWAITING_BUY && !buyDialog.visible && currP)
            triggerBuyDialog(currP->getPosition());

        if (phase == GamePhase::AWAITING_TAX && !taxDialog.visible)
            triggerTaxDialog();

        if (phase == GamePhase::AWAITING_PBM && !pbmDialog.visible)
            triggerPbmDialog();

        if (phase == GamePhase::AWAITING_FESTIVAL && !festivalDialog.visible)
            triggerFestivalDialog();

        if (phase == GamePhase::AWAITING_RENT && !sewaDialog.visible)
            triggerSewaDialog();

        if (phase == GamePhase::SHOW_CARD && !cardDialog.visible)
            triggerCardDialog();

        if (phase == GamePhase::AUCTION && !auctionDialog.visible)
            triggerAuctionDialog();

        if (phase == GamePhase::AWAITING_JAIL && !jailDialog.visible)
            triggerJailDialog();

        if (phase == GamePhase::BANKRUPTCY && !bankruptcyDialog.visible && !bankruptcyDialog.notifVisible)
            triggerBankruptcyDialog();

        if (phase == GamePhase::AWAITING_DROP_SKILL_CARD && !dropSkillCardDialog.visible)
            triggerDropSkillCardDialog();
    }

    // Lelang tetap jalan tanpa diblokir
    if (phase == GamePhase::PLAYER_TURN && !gs.getPendingAuctionQueue().empty() && !auctionDialog.visible)
    {
        Property *next = gs.popPendingAuction();
        if (next)
            gm->startAuction(next, nullptr);
    }
}

void GameScreen::syncDiceResult()
{
    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    Dice *dice = gm->getState().getDice();
    if (!dice)
        return;

    int v1 = dice->getDaduVal1();
    int v2 = dice->getDaduVal2();

    bool justRolled = gm->getState().getHasRolled() && !diceState.hasRolled;
    bool valChanged = (v1 != diceState.val1 || v2 != diceState.val2);

    if ((valChanged || justRolled) && v1 > 0)
    {
        diceState.val1 = v1;
        diceState.val2 = v2;
        diceState.animating = true;
        diceState.animTimer = 0.f;
    }
    diceState.hasRolled = gm->getState().getHasRolled();
}