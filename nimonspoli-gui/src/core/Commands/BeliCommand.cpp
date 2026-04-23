#include "BeliCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Bank/Bank.hpp"
#include <iostream>

BeliCommand::BeliCommand(Player* buyer, Property* property, Bank* bank, bool buy)
    : buyer(buyer), property(property), bank(bank), playerChoseToBuy(buy) {}

void BeliCommand::execute(GameMaster& gm) {
    if (!buyer || !property || !bank) return;

    // Pastikan properti masih milik Bank saat command dieksekusi
    if (property->getStatus() != PropertyStatus::BANK) return;

    if (playerChoseToBuy) {
        // ── Beli ─────────────────────────────────────────────────────────
        int price = property->getPurchasePrice();

        if (!buyer->canAfford(price)) {
            // Tidak mampu bayar → langsung lelang
            gm.log(buyer->getUsername(), "BELI",
                   "Tidak mampu beli " + property->getName() +
                   " (M" + std::to_string(price) + ") → lelang");
            gm.startAuction(property, buyer);
            return;
        }

        buyer->setBalance(buyer->getBalance() - price);
        property->setOwner(buyer->getUsername());
        property->setStatus(PropertyStatus::OWNED);
        buyer->addProperty(property);

        gm.log(buyer->getUsername(), "BELI",
               "Beli " + property->getName() +
               ": -M" + std::to_string(price));

        gm.getState().setPhase(GamePhase::PLAYER_TURN);

    } else {
        // ── Skip → lelang otomatis ────────────────────────────────────────
        gm.log(buyer->getUsername(), "BELI",
               "Skip " + property->getName() + " → lelang");
        gm.startAuction(property, buyer);
    }
}