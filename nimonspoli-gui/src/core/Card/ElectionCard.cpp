#include "ElectionCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

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
    GameMaster* gm = gs.getGameMaster();
    for (Player *other : players)
    {
        if (other == &p)
            continue;
        // Cek apakah player akan Bankrupt? <-- belum dihandle
        if (p.getBalance() < amountPerPlayer) {
            // p.setStatus(PlayerStatus::BANKRUPT);
            if (p.getStatus() == PlayerStatus::BANKRUPT) {
                break;
            }
            // Handle Bankrupt?
            gm->handleDebtPayment(&p, amountPerPlayer, other);
        }
        // p -= amountPerPlayer;
        // *other += amountPerPlayer;
    }
}