#include "FreeFromJailCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

FreeFromJailCard::FreeFromJailCard() : ChanceCard("Keluar Penjara.")
{
}

FreeFromJailCard::~FreeFromJailCard()
{
}

void FreeFromJailCard::execute(Player &p, GameState &gs)
{
    if (p.isInJail())
    {
        p.releaseFromJail();
    }
}