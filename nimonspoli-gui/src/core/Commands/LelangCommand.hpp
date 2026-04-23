#ifndef LELANGCOMMAND_HPP
#define LELANGCOMMAND_HPP

#include "Command.hpp"

// Forward declarations
class Player;
class Property;
class Bank;

// ─────────────────────────────────────────────
//  LelangCommand (Otomatis)
//
//  Dipicu oleh:
//    1. BeliCommand saat pemain skip atau tidak mampu beli Street
//    2. GameMaster::handleBankruptcy() saat pemain bangkrut ke Bank
//
//  Yang dilakukan execute():
//    - Panggil AuctionManager::setupAuction() dengan initiator
//    - Set phase ke AUCTION
//    - GUI (drawAuctionDialog) mengambil alih interaksi pemain
//    - Saat lelang selesai (isAuctionOver()), GameScreen push
//      LelangCommand lagi dengan flag finalize, ATAU GameScreen
//      langsung panggil closeAuction() via guiManager
//
//  Catatan desain:
//    Tidak ada std::cin di sini. Semua interaksi (BID/PASS) dilakukan
//    melalui tombol di drawAuctionDialog(), yang push BidCommand atau
//    PassCommand — ATAU lebih sederhana: drawAuctionDialog langsung
//    memanggil am->placeBid() / am->passBid() dan cek isAuctionOver()
//    setiap frame, lalu push LelangFinalizeCommand saat selesai.
//
//    Untuk M2, pola yang dipakai: execute() hanya setup + set phase.
//    Finalisasi (closeAuction) dipanggil dari drawAuctionDialog saat
//    isAuctionOver() == true.
// ─────────────────────────────────────────────
class LelangCommand : public Command {
private:
    Property* property;   // properti yang dilelang (bukan PropertyTile*)
    Player*   initiator;  // pemain yang memicu lelang (tidak ikut di giliran pertama)
    Bank*     bank;

public:
    LelangCommand(Property* property, Player* initiator, Bank* bank);

    void execute(GameMaster& gm) override;
};

#endif