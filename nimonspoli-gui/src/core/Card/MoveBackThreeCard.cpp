#include "MoveBackThreeCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"

MoveToBackThreeCard::MoveToBackThreeCard() : ChanceCard("Mundur 3 petak.")
{
}

MoveToBackThreeCard::~MoveToBackThreeCard()
{
}

void MoveToBackThreeCard::execute(Player &p, GameState &gs)
{
    GameMaster *gm = gs.getGameMaster();
    if (gm)
    {
        gm->movePlayer(&p, -3);
    }
}