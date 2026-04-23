#include "LelangCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../Property/Property.hpp"
#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"

LelangCommand::LelangCommand(Property* property, Player* initiator, Bank* bank)
    : property(property), initiator(initiator), bank(bank) {}

void LelangCommand::execute(GameMaster& gm) {
    if (!property || !initiator || !bank) return;

    // Properti harus masih milik Bank agar bisa dilelang
    // (kecuali kasus bangkrut — ditangani GameMaster sebelum push command ini)
    AuctionManager* am = gm.getState().getAuctionManager();
    if (!am) return;

    // Setup lelang: tentukan urutan peserta, mulai dari pemain setelah initiator
    am->setupAuction(property, initiator, gm.getState().getPlayers());

    gm.log(initiator->getUsername(), "LELANG",
           "Lelang dimulai untuk " + property->getName());

    // Set phase ke AUCTION — GUI (drawAuctionDialog) mengambil alih dari sini.
    // Interaksi BID/PASS dilakukan via tombol di GameScreen.
    // Finalisasi (closeAuction) dipanggil dari drawAuctionDialog saat
    // AuctionManager::isAuctionOver() == true.
    gm.getState().setPhase(GamePhase::AUCTION);
}