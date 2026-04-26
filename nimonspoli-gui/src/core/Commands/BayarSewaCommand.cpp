#include "BayarSewaCommand.hpp"

#include "../Board/Board.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Exceptions/FinancialException.hpp"

#include <iostream>
#include <stdexcept>


BayarSewaCommand::BayarSewaCommand(Player *lander,
                                   PropertyTile *tile,
                                   int diceTotal,
                                   TransactionLogger *logger,
                                   int turn)
    : lander(lander), tile(tile), diceTotal(diceTotal),
      logger(logger), turn(turn) {}


void BayarSewaCommand::execute(GameMaster &gm)
{
    if (!lander || !tile)
        return;

    Property *prop = tile->getProperty();
    if (!prop)
        return;

    // Properti MORTGAGED → tidak ada sewa
    if (prop->getStatus() == PropertyStatus::MORTGAGED)
    {
        std::cout << "Kamu mendarat di " << prop->getName()
                  << " (" << prop->getCode() << "), milik "
                  << prop->getOwnerId() << ".\n"
                  << "Properti ini sedang digadaikan [M]. "
                     "Tidak ada sewa yang dikenakan.\n";
        return;
    }

    // Cari objek owner dari GameState
    Player *owner = gm.getState().getPlayerById(prop->getOwnerId());
    if (!owner)
        return;

    // Hitung sewa
    int rent = calculateRent(gm);

    // Tampilkan info mendarat
    std::cout << "Kamu mendarat di " << prop->getName()
              << " (" << prop->getCode() << "), milik "
              << owner->getUsername() << "!\n";

    // Informasi kondisi properti (Street: tampilkan jumlah bangunan)
    StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
    if (sp)
    {
        if (sp->gethasHotel())
            std::cout << "Kondisi : Hotel\n";
        else if (sp->getBuildingCount() > 0)
            std::cout << "Kondisi : " << sp->getBuildingCount() << " rumah\n";
        else
            std::cout << "Kondisi : Tanah kosong\n";

        if (sp->getFestivalMultiplier() > 1)
            std::cout << "Festival aktif (x" << sp->getFestivalMultiplier()
                      << ", sisa " << sp->getFestivalDuration() << " giliran)\n";
    }

    std::cout << "Sewa     : M" << rent << "\n";

    // Proses pembayaran
    processPayment(gm, owner, rent);
}

int BayarSewaCommand::calculateRent(GameMaster &gm) const
{
    Property *prop = tile->getProperty();

    // ── Street ──────────────────────────────
    StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
    if (sp)
    {
        bool monopoly = gm.hasMonopoly(
            gm.getState().getPlayerById(prop->getOwnerId()),
            prop->getColorGroup());
        return sp->calculateRentPrice(diceTotal, 1, monopoly);
    }

    // ── Railroad ────────────────────────────
    RailroadProperty *rp = dynamic_cast<RailroadProperty *>(prop);
    if (rp)
    {
        int rrCount = gm.countPlayerRailroads(
            gm.getState().getPlayerById(prop->getOwnerId()));
        return rp->calculateRentPrice(diceTotal, rrCount, false);
    }

    // ── Utility ─────────────────────────────
    UtilityProperty *up = dynamic_cast<UtilityProperty *>(prop);
    if (up)
    {
        int utilCount = gm.countPlayerUtilities(
            gm.getState().getPlayerById(prop->getOwnerId()));
        return up->calculateRentPrice(diceTotal, utilCount, false);
    }

    return 0;
}

void BayarSewaCommand::processPayment(GameMaster &gm, Player *owner, int rent)
{
    Property *prop = tile->getProperty();

    int landerBefore = lander->getBalance();
    int ownerBefore = owner->getBalance();

    if (lander->canAfford(rent))
    {
        // Pembayaran normal
        *lander -= rent;
        *owner += rent;

        std::cout << "Uang kamu  : M" << landerBefore
                  << " -> M" << lander->getBalance() << "\n"
                  << "Uang " << owner->getUsername()
                  << " : M" << ownerBefore
                  << " -> M" << owner->getBalance() << "\n";

        if (logger)
        {
            logger->addLog(turn, lander->getUsername(), "SEWA",
                           "Bayar M" + std::to_string(rent) +
                               " ke " + owner->getUsername() +
                               " (" + prop->getCode() + ")");
        }
    }
    else
    {
        // Tidak mampu bayar → trigger debt handling
        std::cout << "Kamu tidak mampu membayar sewa penuh! (M" << rent << ")\n"
                  << "Uang kamu saat ini: M" << landerBefore << "\n";

        if (logger)
        {
            logger->addLog(turn, lander->getUsername(), "SEWA_GAGAL",
                           "Tidak mampu bayar M" + std::to_string(rent) +
                               " ke " + owner->getUsername() +
                               " (" + prop->getCode() + ")");
        }

        int status = gm.handleDebtPayment(lander, rent, owner);
        if (status == 2) {
            gm.handleBankruptcy(lander, owner);
        }
    }
}