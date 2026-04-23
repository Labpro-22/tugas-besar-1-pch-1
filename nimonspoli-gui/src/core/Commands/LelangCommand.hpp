#ifndef LELANGCOMMAND_HPP
#define LELANGCOMMAND_HPP

#include "Command.hpp"

class Player;
class Property;

// ─────────────────────────────────────────────
//  LelangCommand (Otomatis)
//
//  Dipicu oleh:
//    1. BeliCommand saat pemain skip atau tidak mampu beli Street
//    2. GameMaster::handleBankruptcy() saat pemain bangkrut ke Bank
//
//  execute() hanya melakukan setup + set phase ke AUCTION.
//  Interaksi BID/PASS dan finalisasi (closeAuction) dihandle
//  sepenuhnya oleh GUI (drawAuctionDialog).
// ─────────────────────────────────────────────
class LelangCommand : public Command {
private:
    Property* property;  // properti yang dilelang
    Player*   initiator; // pemain yang memicu lelang (tidak ikut giliran pertama)

public:
    LelangCommand(Property* property, Player* initiator);
    void execute(GameMaster& gm) override;
};

#endif