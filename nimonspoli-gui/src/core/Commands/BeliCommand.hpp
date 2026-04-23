#ifndef BELICOMMAND_HPP
#define BELICOMMAND_HPP

#include "Command.hpp"
#include <string>

class Player;
class Property;

class BeliCommand : public Command {
private:
    Player*   currentPlayer;
    Property* property;
    bool      playerChoseToBuy; // ditentukan GUI sebelum execute()

public:
    BeliCommand(Player* p, Property* prop, bool playerChoseToBuy);
    void execute(GameMaster& gm) override;
};

#endif