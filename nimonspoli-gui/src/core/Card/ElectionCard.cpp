#include "ElectionCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

ElectionCard::ElectionCard() : GeneralFundCard("Anda mau nyaleg. Bayar M200 kepada setiap pemain."), amountPerPlayer(200)
{
}

ElectionCard::~ElectionCard()
{
}

int ElectionCard::getAmountPerPlayer() const
{
    return amountPerPlayer;
}

void ElectionCard::execute(Player &p, GameState &gs)
{
    vector<Player *> players = gs.getActivePlayers();
    for (Player *other : players)
    {
        if (other == &p)
            continue;
        p -= amountPerPlayer;
        *other += amountPerPlayer;
    }
}