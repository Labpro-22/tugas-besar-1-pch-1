#include "BangunCommand.hpp"

#include "../Board/Board.hpp"
#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"
#include "../Property/StreetProperty.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Exceptions/FinancialException.hpp"

#include <iostream>
#include <algorithm>
#include <climits>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

BangunCommand::BangunCommand(Player *player,
                             Bank *bank,
                             TransactionLogger *logger,
                             int turn)
    : player(player), bank(bank), logger(logger), turn(turn),
      guiTargetProp(nullptr), guiIsHotel(false) {}

// Constructor GUI: langsung bangun properti tertentu tanpa prompt CLI
BangunCommand::BangunCommand(Player *player,
                             StreetProperty *targetProp,
                             bool isHotel,
                             TransactionLogger *logger,
                             int turn)
    : player(player), bank(nullptr), logger(logger), turn(turn),
      guiTargetProp(targetProp), guiIsHotel(isHotel) {}

// ─────────────────────────────────────────────
//  execute  — entry point
// ─────────────────────────────────────────────

void BangunCommand::execute(GameMaster &gm)
{
    if (!player)
        return;

    // ── GUI mode: langsung bangun/upgrade properti yang sudah dipilih ──
    if (guiTargetProp != nullptr)
    {
        if (guiIsHotel)
            upgradeToHotel(guiTargetProp,
                           guiTargetProp->getName() + " (" + guiTargetProp->getCode() + ")",
                           gm);
        else
            buildHouse(guiTargetProp,
                       guiTargetProp->getName() + " (" + guiTargetProp->getCode() + ")",
                       gm);
        return;
    }

    // 1. Kumpulkan color group yang eligible
    auto groups = collectEligibleGroups(gm);

    if (groups.empty())
    {
        std::cout << "Tidak ada color group yang memenuhi syarat untuk dibangun.\n"
                  << "Kamu harus memiliki seluruh petak dalam satu color group "
                     "terlebih dahulu.\n";
        return;
    }

    // 2. Tampilkan pilihan color group
    std::cout << "\n=== Color Group yang Memenuhi Syarat ===\n";
    displayGroups(groups);
    std::cout << "\nUang kamu saat ini : M" << player->getBalance() << "\n";

    // Buat indeks untuk navigasi pilihan
    std::vector<std::string> groupKeys;
    groupKeys.reserve(groups.size());
    for (auto &kv : groups)
        groupKeys.push_back(kv.first);

    std::cout << "Pilih nomor color group (0 untuk batal): ";
    int groupChoice = 0;
    std::cin >> groupChoice;
    std::cin.ignore();

    if (groupChoice <= 0 || groupChoice > static_cast<int>(groupKeys.size()))
    {
        std::cout << "Dibatalkan.\n";
        return;
    }

    const std::string &chosenGroup = groupKeys[groupChoice - 1];
    std::vector<TileEntry> &groupEntries = groups[chosenGroup];

    // 3. Loop: tampilkan detail group dan minta pilihan petak
    //    Pemain bisa membangun sa  tu per satu selama masih mau
    while (true)
    {
        std::cout << "\nColor group [" << chosenGroup << "]:\n";
        displayGroupDetail(chosenGroup, groupEntries);

        // Cek apakah semua siap upgrade hotel
        bool hotelReady = allReadyForHotel(groupEntries);
        if (hotelReady)
        {
            std::cout << "Seluruh color group [" << chosenGroup
                      << "] sudah memiliki 4 rumah. Siap di-upgrade ke hotel!\n";
        }

        // Kumpulkan petak yang bisa dibangun sekarang
        std::vector<int> buildableIdx;
        for (int i = 0; i < static_cast<int>(groupEntries.size()); ++i)
        {
            const TileEntry &te = groupEntries[i];
            StreetProperty *sp = te.getProp();
            // Sudah hotel → skip
            if (sp->gethasHotel())
                continue;
            // Jika hotel ready, semua yang belum hotel bisa upgrade
            if (hotelReady)
            {
                buildableIdx.push_back(i);
                continue;
            }
            // Aturan pemerataan
            if (canBuildOnTile(te, groupEntries))
                buildableIdx.push_back(i);
        }

        if (buildableIdx.empty())
        {
            std::cout << "Tidak ada petak yang dapat dibangun saat ini.\n";
            break;
        }

        std::cout << "Pilih petak (0 untuk batal): ";
        int tileChoice = 0;
        std::cin >> tileChoice;
        std::cin.ignore();

        if (tileChoice <= 0)
        {
            std::cout << "Selesai membangun.\n";
            break;
        }

        // Validasi: pilihan harus ada di buildableIdx
        // Input tileChoice mengacu ke nomor tampilan (1-based dari groupEntries)
        if (tileChoice > static_cast<int>(groupEntries.size()))
        {
            std::cout << "Pilihan tidak valid.\n";
            continue;
        }

        int entryIdx = tileChoice - 1;
        // Pastikan index ada di buildableIdx
        bool isAllowed = false;
        for (int bi : buildableIdx)
            if (bi == entryIdx)
            {
                isAllowed = true;
                break;
            }

        if (!isAllowed)
        {
            std::cout << "Petak ini tidak memenuhi syarat pemerataan saat ini.\n";
            continue;
        }

        TileEntry &chosen = groupEntries[entryIdx];
        StreetProperty *sp = chosen.getProp();

        if (hotelReady && !sp->gethasHotel())
        {
            // Upgrade ke hotel
            upgradeToHotel(sp, chosen.getName() + " (" + chosen.getCode() + ")", gm);
        }
        else
        {
            // Bangun rumah
            buildHouse(sp, chosen.getName() + " (" + chosen.getCode() + ")", gm);
        }

        // Setelah aksi, cek lagi apakah masih ada yang bisa dibangun
        bool anyBuildable = false;
        bool newHotelReady = allReadyForHotel(groupEntries);
        for (int i = 0; i < static_cast<int>(groupEntries.size()); ++i)
        {
            const TileEntry &te = groupEntries[i];
            if (te.getProp()->gethasHotel())
                continue;
            if (newHotelReady)
            {
                anyBuildable = true;
                break;
            }
            if (canBuildOnTile(te, groupEntries))
            {
                anyBuildable = true;
                break;
            }
        }
        if (!anyBuildable)
        {
            std::cout << "Semua petak dalam [" << chosenGroup
                      << "] sudah mencapai level maksimal.\n";
            break;
        }
    }
}

// ─────────────────────────────────────────────
//  collectEligibleGroups
// ─────────────────────────────────────────────

std::map<std::string, std::vector<BangunCommand::TileEntry>>
BangunCommand::collectEligibleGroups(GameMaster &gm) const
{
    std::map<std::string, std::vector<TileEntry>> result;
    Board *board = gm.getState().getBoard();
    if (!board)
        return result;

    // Kumpulkan semua StreetProperty milik player, kelompokkan by colorGroup
    for (int i = 0; i < board->getSize(); ++i)
    {
        PropertyTile *pt = dynamic_cast<PropertyTile *>(board->getTile(i));
        if (!pt)
            continue;

        Property *prop = pt->getProperty();
        if (!prop)
            continue;

        StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
        if (!sp)
            continue;

        if (prop->getOwnerId() != player->getUsername())
            continue;
        if (prop->getStatus() == PropertyStatus::MORTGAGED)
            continue;

        TileEntry entry(prop->getCode(), prop->getName(), sp);
        result[prop->getColorGroup()].push_back(entry);
    }

    // Filter: hanya group dengan monopoli penuh
    // (jumlah yang dimiliki player == total petak di group di papan)
    std::map<std::string, int> totalInGroup;
    for (int i = 0; i < board->getSize(); ++i)
    {
        PropertyTile *pt = dynamic_cast<PropertyTile *>(board->getTile(i));
        if (!pt)
            continue;
        Property *prop = pt->getProperty();
        if (!prop)
            continue;
        if (!dynamic_cast<StreetProperty *>(prop))
            continue;
        totalInGroup[prop->getColorGroup()]++;
    }

    std::map<std::string, std::vector<TileEntry>> eligible;
    for (auto &kv : result)
    {
        const std::string &cg = kv.first;
        std::vector<TileEntry> &entries = kv.second;

        // Cek monopoli
        if (static_cast<int>(entries.size()) != totalInGroup[cg])
            continue;

        // Cek apakah semua petak sudah hotel (tidak perlu ditampilkan)
        bool allHotel = true;
        for (auto &te : entries)
            if (!te.getProp()->gethasHotel())
            {
                allHotel = false;
                break;
            }
        if (allHotel)
            continue;

        eligible[cg] = entries;
    }

    return eligible;
}

// ─────────────────────────────────────────────
//  canBuildOnTile
//  Aturan pemerataan: petak ini boleh dibangun jika
//  jumlah bangunannya == minimum di group saat ini
// ─────────────────────────────────────────────

bool BangunCommand::canBuildOnTile(const TileEntry &entry,
                                   const std::vector<TileEntry> &group) const
{
    // Sudah hotel → tidak bisa dibangun
    if (entry.getProp()->gethasHotel())
        return false;

    // Cari minimum bangunan di group (tidak termasuk yang sudah hotel)
    int minBuildings = INT_MAX;
    for (const TileEntry &te : group)
    {
        if (te.getProp()->gethasHotel())
            continue;
        int count = te.getProp()->getBuildingCount();
        if (count < minBuildings)
            minBuildings = count;
    }

    // Boleh bangun jika bangunan petak ini == minimum
    return (entry.getProp()->getBuildingCount() == minBuildings);
}

// ─────────────────────────────────────────────
//  allReadyForHotel
// ─────────────────────────────────────────────

bool BangunCommand::allReadyForHotel(const std::vector<TileEntry> &group) const
{
    for (const TileEntry &te : group)
    {
        if (te.getProp()->gethasHotel())
            continue; // sudah hotel, tidak perlu cek
        if (te.getProp()->getBuildingCount() < 4)
            return false;
    }
    return true;
}

// ─────────────────────────────────────────────
//  displayGroups
// ─────────────────────────────────────────────

void BangunCommand::displayGroups(
    const std::map<std::string, std::vector<TileEntry>> &groups) const
{
    int idx = 1;
    for (const auto &kv : groups)
    {
        std::cout << idx++ << ". [" << kv.first << "]\n";
        for (const TileEntry &te : kv.second)
        {
            StreetProperty *sp = te.getProp();
            std::cout << "   - " << te.getName() << " (" << te.getCode() << ") : ";
            if (sp->gethasHotel())
                std::cout << "Hotel (Harga hotel: M" << sp->getHotelUpgCost() << ")";
            else
                std::cout << sp->getBuildingCount()
                          << " rumah (Harga rumah: M" << sp->getHouseUpgCost() << ")";
            std::cout << "\n";
        }
    }
}

// ─────────────────────────────────────────────
//  displayGroupDetail
// ─────────────────────────────────────────────

void BangunCommand::displayGroupDetail(
    const std::string &colorGroup,
    const std::vector<TileEntry> &group) const
{
    bool hotelReady = allReadyForHotel(group);

    for (int i = 0; i < static_cast<int>(group.size()); ++i)
    {
        const TileEntry &te = group[i];
        StreetProperty *sp = te.getProp();

        std::cout << i + 1 << ". " << te.getName() << " (" << te.getCode() << ") : ";

        if (sp->gethasHotel())
        {
            std::cout << "Hotel <- sudah maksimal, tidak dapat dibangun";
        }
        else if (hotelReady)
        {
            std::cout << "4 rumah <- siap upgrade ke hotel"
                      << " (Biaya: M" << sp->getHotelUpgCost() << ")";
        }
        else
        {
            std::cout << sp->getBuildingCount() << " rumah";
            if (canBuildOnTile(te, group))
                std::cout << " <- dapat dibangun"
                          << " (Harga rumah: M" << sp->getHouseUpgCost() << ")";
        }
        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  buildHouse
// ─────────────────────────────────────────────

void BangunCommand::buildHouse(StreetProperty *prop,
                               const std::string &propName,
                               GameMaster &gm)
{
    int cost = prop->getHouseUpgCost();

    if (!player->canAfford(cost))
    {
        std::cout << "Uang tidak cukup untuk membangun rumah di " << propName
                  << ". Biaya: M" << cost
                  << " | Uang kamu: M" << player->getBalance() << "\n";
        throw InsufficientFundsException(player->getUsername(),
                                         "membangun rumah di " + propName);
    }

    *player -= cost;
    prop->buildHouse();

    std::cout << "Kamu membangun 1 rumah di " << propName
              << ". Biaya: M" << cost << "\n"
              << "Uang tersisa: M" << player->getBalance() << "\n"
              << "- " << propName << " : " << prop->getBuildingCount() << " rumah\n";

    if (logger)
        logger->addLog(turn, player->getUsername(), "BANGUN",
                       "Bangun rumah di " + propName +
                           " (" + std::to_string(prop->getBuildingCount()) + " rumah)" +
                           " seharga M" + std::to_string(cost));
}

// ─────────────────────────────────────────────
//  upgradeToHotel
// ─────────────────────────────────────────────

void BangunCommand::upgradeToHotel(StreetProperty *prop,
                                   const std::string &propName,
                                   GameMaster &gm)
{
    int cost = prop->getHotelUpgCost();

    // Dalam mode GUI, konfirmasi sudah dilakukan di BangunDialog sebelum command ini dipanggil.
    // Dalam mode CLI, upgrade langsung dieksekusi tanpa prompt (pemain sudah memilih properti ini).

    if (!player->canAfford(cost))
    {
        std::cout << "Uang tidak cukup untuk upgrade hotel di " << propName
                  << ". Biaya: M" << cost
                  << " | Uang kamu: M" << player->getBalance() << "\n";
        throw InsufficientFundsException(player->getUsername(),
                                         "upgrade hotel di " + propName);
    }

    *player -= cost;
    prop->upgToHotel();

    // Kembalikan 4 rumah ke Bank (stok tidak terbatas per spek,
    // tapi tetap sinkronkan jika Bank melacaknya)
    // bank->setHousesCount(bank->getHousesCount() + 4); // uncomment jika stok aktif

    std::cout << propName << " di-upgrade ke Hotel!\n"
              << "Uang tersisa: M" << player->getBalance() << "\n";

    if (logger)
        logger->addLog(turn, player->getUsername(), "BANGUN",
                       "Upgrade hotel di " + propName +
                           " seharga M" + std::to_string(cost));
}