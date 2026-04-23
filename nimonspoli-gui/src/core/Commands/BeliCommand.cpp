#include "BeliCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"   // untuk deteksi tipe via dynamic_cast
#include "../Bank/Bank.hpp"
#include <iostream>

BeliCommand::BeliCommand(Player* buyer, Property* property, Bank* bank, bool buy)
    : buyer(buyer), property(property), bank(bank), playerChoseToBuy(buy) {}

void BeliCommand::execute(GameMaster& gm) {
    if (!buyer || !property || !bank) return;

    // Properti harus masih milik Bank saat command dieksekusi
    if (property->getStatus() != PropertyStatus::BANK) return;

    // ── Deteksi tipe properti ─────────────────────────────────────────────
    // StreetProperty   → ada prompt beli/skip
    // RailroadProperty → gratis, langsung berpindah
    // UtilityProperty  → gratis, langsung berpindah
    bool isStreet = (dynamic_cast<StreetProperty*>(property) != nullptr);

    if (!isStreet) {
        // ── Railroad / Utility: gratis, langsung milik pemain ─────────────
        property->setOwner(buyer->getUsername());
        property->setStatus(PropertyStatus::OWNED);
        buyer->addProperty(property);

        gm.log(buyer->getUsername(), "BELI",
               "Mendapat " + property->getName() + " gratis (Railroad/Utility)");

        gm.getState().setPhase(GamePhase::PLAYER_TURN);
        return;
    }

    // ── Street ────────────────────────────────────────────────────────────
    int price = property->getPurchasePrice();

    if (playerChoseToBuy) {
        // Pemain memilih beli — cek dulu apakah mampu
        if (!buyer->canAfford(price)) {
            // Tidak mampu bayar → langsung lelang tanpa konfirmasi ulang
            gm.log(buyer->getUsername(), "BELI",
                   "Tidak mampu beli " + property->getName() +
                   " (M" + std::to_string(price) + ") → lelang");
            gm.getState().setPhase(GamePhase::PLAYER_TURN);
            gm.startAuction(property, buyer);
            return;
        }

        // Transaksi pembelian
        *buyer -= price;                          
        
        property->setOwner(buyer->getUsername());
        property->setStatus(PropertyStatus::OWNED);
        buyer->addProperty(property);

        gm.log(buyer->getUsername(), "BELI",
               "Beli " + property->getName() +
               ": -M" + std::to_string(price) +
               " | Sisa: M" + std::to_string(buyer->getBalance()));

        gm.getState().setPhase(GamePhase::PLAYER_TURN);

    } else {
        // ── Skip → lelang otomatis ────────────────────────────────────────
        gm.log(buyer->getUsername(), "BELI",
               "Skip " + property->getName() + " → lelang");
        gm.getState().setPhase(GamePhase::PLAYER_TURN);
        gm.startAuction(property, buyer);
    }
}