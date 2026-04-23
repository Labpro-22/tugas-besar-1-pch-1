#pragma once
#include "Command.hpp"
#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

class GameMaster;

class BankruptCommand : public Command {
private: 
    GameMaster& gm;
    GameState& state;
    Player* from;
    Player* to;
    int debt;
    bool sellOverMortgage;
    int propToSell;

public: 
    BankruptCommand(GameMaster& gm, GameState& gs, Player* from, Player* to, int debt, bool sellOverMortgage, int propToSell);

    // Handle Bankruptcy and Liquidation
    void execute(GameMaster& gm) override;
};