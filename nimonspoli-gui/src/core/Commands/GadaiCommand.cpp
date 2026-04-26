#include "GadaiCommand.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include <iostream>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

GadaiCommand::GadaiCommand(Player *p, Property *prop, bool playerConfirmedSell)
    : currentPlayer(p), selectedProperty(prop),
      playerConfirmedSell(playerConfirmedSell) {}

// ─────────────────────────────────────────────
//  Helper: cek apakah ada bangunan di color group milik pemain ini
// ─────────────────────────────────────────────

static bool hasBuildingsInColorGroup(Player *player,
                                     const std::string &colorGroup)
{
    for (Property *prop : player->getProperties())
    {
        if (prop->getColorGroup() != colorGroup)
            continue;
        StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
        if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel()))
            return true;
    }
    return false;
}

// ─────────────────────────────────────────────
//  Helper: jual semua bangunan di color group, return total nilai jual
// ─────────────────────────────────────────────

static int sellAllBuildingsInColorGroup(Player *player,
                                        const std::string &colorGroup)
{
    int total = 0;
    for (Property *prop : player->getProperties())
    {
        if (prop->getColorGroup() != colorGroup)
            continue;
        StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
        if (!sp)
            continue;
        if (sp->getBuildingCount() > 0 || sp->gethasHotel())
        {
            // sellAllBuildings() sudah return setengah harga beli (sesuai spesifikasi)
            int proceeds = sp->sellAllBuildings();
            *player += proceeds;
            total += proceeds;
            std::cout << "[DEBUG] Bangunan " << prop->getName()
                      << " terjual. Menerima M" << proceeds << std::endl;
        }
    }
    return total;
}

// ─────────────────────────────────────────────
//  execute()
// ─────────────────────────────────────────────

void GadaiCommand::execute(GameMaster &gm)
{
    if (!currentPlayer || !selectedProperty)
    {
        std::cerr << "[GadaiCommand] Error: player atau property null." << std::endl;
        return;
    }

    // Hanya properti milik pemain ini yang bisa digadaikan
    if (selectedProperty->getOwnerId() != currentPlayer->getUsername())
    {
        std::cerr << "[GadaiCommand] Properti bukan milik pemain ini." << std::endl;
        return;
    }

    // Properti harus berstatus OWNED (bukan MORTGAGED atau BANK)
    if (selectedProperty->getStatus() != PropertyStatus::OWNED)
    {
        std::cerr << "[GadaiCommand] Properti sudah digadaikan atau bukan milik Bank." << std::endl;
        return;
    }

    // ── Cek bangunan di color group (hanya relevan untuk StreetProperty) ─────
    std::string colorGroup = selectedProperty->getColorGroup();
    bool isStreet = dynamic_cast<StreetProperty *>(selectedProperty) != nullptr;

    if (isStreet && hasBuildingsInColorGroup(currentPlayer, colorGroup))
    {
        if (!playerConfirmedSell)
        {
            // GUI belum dapat konfirmasi — batalkan, GUI akan tampilkan dialog
            std::cout << "[DEBUG] Masih ada bangunan di color group ["
                      << colorGroup << "]. Menunggu konfirmasi jual." << std::endl;
            return;
        }

        // Pemain setuju jual semua bangunan di color group ini
        int totalProceeds = sellAllBuildingsInColorGroup(currentPlayer, colorGroup);
        std::cout << "[DEBUG] Total penerimaan dari penjualan bangunan: M"
                  << totalProceeds << std::endl;
    }

    // ── Proses gadai ──────────────────────────────────────────────────────────
    int mortgageValue = selectedProperty->getMortageValue();

    selectedProperty->setStatus(PropertyStatus::MORTGAGED);
    *currentPlayer += mortgageValue;

    gm.log(currentPlayer->getUsername(), "GADAI",
           selectedProperty->getName() + " digadaikan, menerima M" +
               std::to_string(mortgageValue));

    std::cout << "[DEBUG] " << selectedProperty->getName()
              << " berhasil digadaikan. Menerima M" << mortgageValue
              << ". Saldo: M" << currentPlayer->getBalance() << std::endl;
}
