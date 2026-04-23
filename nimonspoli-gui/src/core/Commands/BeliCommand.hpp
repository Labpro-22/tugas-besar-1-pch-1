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
//  Tiga skenario berdasarkan tipe properti:
//  - Street   : playerChoseToBuy menentukan beli atau lelang
//               Jika tidak mampu → langsung lelang tanpa tanya
//  - Railroad : kepemilikan langsung berpindah GRATIS (playerChoseToBuy diabaikan)
//  - Utility  : kepemilikan langsung berpindah GRATIS (playerChoseToBuy diabaikan)
//
//  Deteksi tipe dilakukan via dynamic_cast ke StreetProperty.
//  Jika bukan StreetProperty → Railroad atau Utility → jalur gratis.
// ─────────────────────────────────────────────
class BeliCommand : public Command {
private:
    Player*   buyer;
    Property* property;
    Bank*     bank;
    bool      playerChoseToBuy; // true = beli (Street), false = skip/lelang

public:
    BeliCommand(Player* buyer, Property* property, Bank* bank, bool buy);

    void execute(GameMaster& gm) override;
};

#endif