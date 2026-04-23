#include "BeliCommand.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include <iostream>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

BeliCommand::BeliCommand(Player* p, Property* prop, bool playerChoseToBuy)
    : currentPlayer(p), property(prop), playerChoseToBuy(playerChoseToBuy) {}

// ─────────────────────────────────────────────
//  execute()
// ─────────────────────────────────────────────

void BeliCommand::execute(GameMaster& gm) {
    if (!currentPlayer || !property) {
        std::cerr << "[BeliCommand] Error: player atau property null." << std::endl;
        return;
    }

    GameState& gs = gm.getGameState();

    // Hanya berlaku untuk properti berstatus BANK
    if (property->getStatus() != PropertyStatus::BANK) {
        std::cerr << "[BeliCommand] Property " << property->getCode()
                  << " bukan milik Bank, skip." << std::endl;
        return;
    }

    // ── Railroad & Utility: akuisisi gratis, langsung tanpa prompt ────────────
    bool isRailroad = dynamic_cast<RailroadProperty*>(property) != nullptr;
    bool isUtility  = dynamic_cast<UtilityProperty*>(property)  != nullptr;

    if (isRailroad || isUtility) {
        property->setOwner(currentPlayer->getUsername());
        property->setStatus(PropertyStatus::OWNED);
        currentPlayer->addProperty(property);

        std::cout << "[DEBUG] " << currentPlayer->getUsername()
                  << " mendapatkan " << property->getName()
                  << " (" << property->getCode() << ") secara gratis." << std::endl;

        gs.setPhase(GamePhase::PLAYER_TURN);
        return;
    }

    // ── Street: bergantung pada pilihan pemain (ditentukan GUI) ───────────────
    int price = property->getPurchasePrice();

    if (playerChoseToBuy && currentPlayer->canAfford(price)) {
        *currentPlayer -= price;
        property->setOwner(currentPlayer->getUsername());
        property->setStatus(PropertyStatus::OWNED);
        currentPlayer->addProperty(property);

        std::cout << "[DEBUG] " << currentPlayer->getUsername()
                  << " membeli " << property->getName()
                  << " seharga M" << price
                  << ". Saldo: M" << currentPlayer->getBalance() << std::endl;

        gs.setPhase(GamePhase::PLAYER_TURN);
    } else {
        // Tidak mau beli atau uang tidak cukup → lelang
        std::cout << "[DEBUG] " << property->getName()
                  << " masuk ke lelang." << std::endl;

        gs.setPhase(GamePhase::AUCTION);
        // GameMaster mendeteksi fase AUCTION dan menjalankan LelangCommand
    }
}