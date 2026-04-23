#ifndef BELICOMMAND_HPP
#define BELICOMMAND_HPP

#include "../Commands/Command.hpp"

class Player;
class Property;
class Bank;

// ─────────────────────────────────────────────
//  BeliCommand
//  Dipicu otomatis saat pemain mendarat di petak
//  properti berstatus BANK.
//
//  - Street   : tampilkan dialog beli/skip
//               skip → startAuction() di GameMaster
//  - Railroad : kepemilikan langsung berpindah (gratis)
//  - Utility  : kepemilikan langsung berpindah (gratis)
// ─────────────────────────────────────────────
class BeliCommand : public Command {
private:
    Player*   buyer;
    Property* property;
    Bank*     bank;
    bool      playerChoseToBuy; // true = beli, false = skip/lelang

public:
    BeliCommand(Player* buyer, Property* property, Bank* bank, bool buy);

    void execute(GameMaster& gm) override;
};

#endif