#include "BirthdayCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

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
    GameMaster *gm = gs.getGameMaster();
    for (Player *other : players)
    {
        if (other == &p)
            continue;
        // Cek apakah player akan Bankrupt? <-- belum dihandle
        if (other->getBalance() < amountPerPlayer) {
            // p.setStatus(PlayerStatus::BANKRUPT);
            // Handle bankrupt by GameMaster
            if (other->getStatus() == PlayerStatus::BANKRUPT) {
                break;
            }
            // Handle 
            gm->handleDebtPayment(&p, amountPerPlayer, other);
        }
        // *other -= amountPerPlayer;
        // p += amountPerPlayer;
    }
}