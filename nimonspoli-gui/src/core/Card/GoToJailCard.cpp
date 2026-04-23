#include "GoToJailCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"

GoToJailCard::GoToJailCard() : ChanceCard("Masuk Penjara")
{
}

GoToJailCard::~GoToJailCard()
{
}

void GoToJailCard::execute(Player &p, GameState &gs)
{
    int jailIdx = gs.getBoard()->findTileIndexByCode("PEN");
    p.setPosition(jailIdx);
    p.goToJail();
}