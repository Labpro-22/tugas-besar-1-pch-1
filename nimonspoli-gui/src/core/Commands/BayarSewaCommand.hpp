#pragma once

#include "./Command.hpp"
#include "../utils/TransactionLogger.hpp"
#include <string>

class Player;
class PropertyTile;

// ─────────────────────────────────────────────
//  BayarSewaCommand
//
//  Dipanggil dari onLanded() masing-masing tile
//  (StreetTile, RailRoadTile, UtilityTile) ketika
//  properti berstatus OWNED dan pemiliknya bukan
//  pemain yang mendarat.
//
//  Konstruktor menerima:
//  - lander      : pemain yang mendarat
//  - tile        : tile yang diinjak (untuk hitung sewa)
//  - diceTotal   : total dadu (dibutuhkan oleh UtilityTile)
//  - logger      : untuk log transaksi
//  - turn        : turn saat ini
// ─────────────────────────────────────────────
class BayarSewaCommand : public Command
{
public:
    BayarSewaCommand(Player *lander,
                     PropertyTile *tile,
                     int diceTotal,
                     TransactionLogger *logger,
                     int turn);

    void execute(GameMaster &gm) override;

private:
    Player *lander;
    PropertyTile *tile;
    int diceTotal;
    TransactionLogger *logger;
    int turn;

    // Hitung sewa sesuai jenis properti via GameMaster helper
    int calculateRent(GameMaster &gm) const;

    // Transfer uang dari lander ke owner, atau trigger kebangkrutan
    void processPayment(GameMaster &gm, Player *owner, int rent);
};
