#include "BeliCommand.hpp"
#include "../Player/Player.hpp"
#include "../Utils/TransactionLogger.hpp"
#include "../Property/Property.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include <iostream>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

BeliCommand::BeliCommand(Player *p, Property *prop, bool playerChoseToBuy)
    : currentPlayer(p), property(prop), playerChoseToBuy(playerChoseToBuy) {}

// ─────────────────────────────────────────────
//  execute()
// ─────────────────────────────────────────────

void BeliCommand::execute(GameMaster &gm)
{
    if (!currentPlayer || !property)
    {
        std::cerr << "[BeliCommand] Error: player atau property null." << std::endl;
        return;
    }

    GameState &gs = gm.getState();

    // Hanya berlaku untuk properti berstatus BANK
    if (property->getStatus() != PropertyStatus::BANK)
    {
        std::cerr << "[BeliCommand] Property " << property->getCode()
                  << " bukan milik Bank, skip." << std::endl;
        return;
    }

    // ── Railroad & Utility: akuisisi gratis, langsung tanpa prompt ────────────

    // ── Street: bergantung pada pilihan pemain (ditentukan GUI) ───────────────
    int originalPrice = property->getPurchasePrice();
    int finalPrice = currentPlayer->applyDiscount(originalPrice);

    if (playerChoseToBuy && currentPlayer->canAfford(finalPrice))
    {
        *currentPlayer -= finalPrice;

        property->setOwner(currentPlayer->getUsername());
        property->setStatus(PropertyStatus::OWNED);
        currentPlayer->addProperty(property);

        std::cout << "[DEBUG] " << currentPlayer->getUsername()
                  << " membeli " << property->getName()
                  << " seharga M" << finalPrice;

        if (finalPrice < originalPrice)
        {
            std::cout << " (harga awal M" << originalPrice
                      << ", diskon " << currentPlayer->getDiscount()
                      << "%)";
        }

        std::cout << ". Saldo: M" << currentPlayer->getBalance() << std::endl;

        // Log ke TransactionLogger
        TransactionLogger *logger = gs.getLogger();
        if (logger)
        {
            std::string detail = "Membeli " + property->getName() + " seharga M" + std::to_string(finalPrice);
            if (finalPrice < originalPrice)
            {
                detail += " (Diskon " + std::to_string(currentPlayer->getDiscount()) + "%)";
            }
            logger->addLog(gs.getCurrTurn(), currentPlayer->getUsername(), "BELI", detail);
        }

        gs.setPhase(GamePhase::PLAYER_TURN);
    }
    else
    {
        std::cout << "[DEBUG] " << property->getName()
                  << " masuk ke lelang." << std::endl;

        gm.startAuction(property, currentPlayer);
    }
}