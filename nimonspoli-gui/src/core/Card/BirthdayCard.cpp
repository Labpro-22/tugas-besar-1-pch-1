#include "BirthdayCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

BirthdayCard::BirthdayCard() : GeneralFundCard("Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain."), amountPerPlayer(100)
{
}

BirthdayCard::~BirthdayCard()
{
}

int BirthdayCard::getAmountPerPlayer() const
{
    return amountPerPlayer;
}

void BirthdayCard::execute(Player &p, GameState &gs)
{
    vector<Player *> players = gs.getActivePlayers();
    for (Player *other : players)
    {
        if (other == &p)
            continue;
        *other -= amountPerPlayer;
        p += amountPerPlayer;
    }
}