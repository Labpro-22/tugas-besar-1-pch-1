#include "TebusCommand.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../GameMaster/GameMaster.hpp"
#include <iostream>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

TebusCommand::TebusCommand(Player* p, Property* prop)
    : currentPlayer(p), selectedProperty(prop) {}

// ─────────────────────────────────────────────
//  execute()
// ─────────────────────────────────────────────

void TebusCommand::execute(GameMaster& gm) {
    if (!currentPlayer || !selectedProperty) {
        std::cerr << "[TebusCommand] Error: player atau property null." << std::endl;
        return;
    }

    // Hanya properti milik pemain ini yang bisa ditebus
    if (selectedProperty->getOwnerId() != currentPlayer->getUsername()) {
        std::cerr << "[TebusCommand] Properti bukan milik pemain ini." << std::endl;
        return;
    }

    // Properti harus berstatus MORTGAGED
    if (selectedProperty->getStatus() != PropertyStatus::MORTGAGED) {
        std::cerr << "[TebusCommand] Properti tidak sedang digadaikan." << std::endl;
        return;
    }

    // Harga tebus = harga beli penuh (sesuai spesifikasi)
    int redeemPrice = selectedProperty->getPurchasePrice();

    if (!currentPlayer->canAfford(redeemPrice)) {
        std::cout << "[DEBUG] " << currentPlayer->getUsername()
                  << " tidak mampu menebus " << selectedProperty->getName()
                  << ". Harga tebus: M" << redeemPrice
                  << " | Saldo: M" << currentPlayer->getBalance() << std::endl;
        return;
    }

    // Proses penebusan
    *currentPlayer -= redeemPrice;
    selectedProperty->setStatus(PropertyStatus::OWNED);

    gm.log(currentPlayer->getUsername(), "TEBUS",
           selectedProperty->getName() + " ditebus, membayar M" +
           std::to_string(redeemPrice));

    std::cout << "[DEBUG] " << selectedProperty->getName()
              << " berhasil ditebus. Membayar M" << redeemPrice
              << ". Saldo: M" << currentPlayer->getBalance() << std::endl;
}