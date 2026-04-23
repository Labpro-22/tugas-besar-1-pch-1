#ifndef GADAICOMMAND_HPP
#define GADAICOMMAND_HPP

#include "Command.hpp"
#include <string>

class Player;
class Property;

class GadaiCommand : public Command {
private:
    Player*   currentPlayer;
    Property* selectedProperty;    // properti yang ingin digadaikan (dipilih GUI)
    bool      playerConfirmedSell; // apakah pemain setuju jual bangunan dulu (dipilih GUI)

public:
    // playerConfirmedSell hanya relevan jika ada bangunan di color group.
    // GUI set true jika pemain menekan "y" pada dialog jual bangunan.
    GadaiCommand(Player* p, Property* prop, bool playerConfirmedSell);

    void execute(GameMaster& gm) override;
};

#endif