#include "BirthdayCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

BirthdayCard::BirthdayCard() : GeneralFundCard("Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain."), amountPerPlayer(100)
{
}

BirthdayCard::BirthdayCard(const string &type, const string &description, int amount) : GeneralFundCard(description), amountPerPlayer(amount)
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
    GameMaster* gm = gs.getGameMaster();

    // Enqueue semua pembayaran dulu: setiap pemain lain harus bayar ke p.
    gs.clearPendingPaymentQueue();
    for (Player* other : gs.getActivePlayers())
    {
        if (other == &p || other->getStatus() == PlayerStatus::BANKRUPT)
            continue;
        // Debtor = other, creditor = p (pemain ulang tahun)
        gs.addToPendingPaymentQueue(other, &p, amountPerPlayer);
    }

    // Jalankan pembayaran pertama
    if (gm) gm->processNextCardPayment();
}
