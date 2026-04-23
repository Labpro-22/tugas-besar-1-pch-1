#ifndef TEBUSCOMMAND_HPP
#define TEBUSCOMMAND_HPP

#include "Command.hpp"

class Player;
class Property;

class TebusCommand : public Command {
private:
    Player*   currentPlayer;
    Property* selectedProperty; // properti yang ingin ditebus (dipilih GUI)

public:
    TebusCommand(Player* p, Property* prop);
    void execute(GameMaster& gm) override;
};

#endif