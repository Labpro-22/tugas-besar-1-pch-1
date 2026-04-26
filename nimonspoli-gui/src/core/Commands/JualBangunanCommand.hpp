#pragma once
#include "Command.hpp"
#include <string>

class Player;
class StreetProperty;

class JualBangunanCommand : public Command {
private:
    Player* player;
    StreetProperty* prop;
    int turn;

public:
    JualBangunanCommand(Player* p, StreetProperty* sp, int turn);
    void execute(GameMaster& gm) override;
};
