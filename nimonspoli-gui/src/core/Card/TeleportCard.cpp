#include "TeleportCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"

TeleportCard::TeleportCard() : SkillCard("Pindah ke petak manapun", "TeleportCard"), targetPosition(-999)
{
}

TeleportCard::~TeleportCard()
{
}

void TeleportCard::setTargetPosition(int pos, GameState &gs)
{
    int boardSize = gs.getBoard()->getSize();
    if (pos >= 0 && pos < boardSize)
        targetPosition = pos;
}

int TeleportCard::getTargetPosition() const { return targetPosition; }

void TeleportCard::execute(Player &p, GameState &gs)
{
    if (targetPosition == -999)
        return;
    GameMaster *gm = gs.getGameMaster();
    if (gm)
        gm->teleportPlayer(&p, targetPosition);
}