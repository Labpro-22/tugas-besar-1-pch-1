#include "LelangCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../Property/Property.hpp"
#include "../Player/Player.hpp"

LelangCommand::LelangCommand(Property* property, Player* initiator)
    : property(property), initiator(initiator) {}

void LelangCommand::execute(GameMaster& gm) {
    if (!property || !initiator) return;

    AuctionManager* am = gm.getState().getAuctionManager();
    if (!am) return;

    // Setup urutan peserta; initiator tidak ikut di giliran pertama
    am->setupAuction(property, initiator, gm.getState().getPlayers());

    gm.log(initiator->getUsername(), "LELANG",
           "Lelang dimulai untuk " + property->getName());

    // Set phase ke AUCTION — GUI mengambil alih dari sini
    gm.getState().setPhase(GamePhase::AUCTION);
}