#include "ElectionCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

ElectionCard::ElectionCard() : GeneralFundCard("Anda mau nyaleg. Bayar M200 kepada setiap pemain."), amountPerPlayer(200)
{
}

ElectionCard::ElectionCard(const string &type, const string &description, int amount) : GeneralFundCard(description), amountPerPlayer(amount)
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
    GameMaster* gm = gs.getGameMaster();
    if (p.getStatus() == PlayerStatus::BANKRUPT) return;

    // Enqueue semua pembayaran dulu, lalu proses satu per satu via queue.
    // Ini mencegah loop melanjutkan ke pemain berikutnya saat dialog masih aktif.
    gs.clearPendingPaymentQueue();
    for (Player* other : gs.getActivePlayers())
    {
        if (other == &p)
            continue;
        // Pembayar = p (pemain yang kena kartu), penerima = other
        gs.addToPendingPaymentQueue(&p, other, amountPerPlayer);
    }

    // Jalankan pembayaran pertama
    if (gm) gm->processNextCardPayment();
}
