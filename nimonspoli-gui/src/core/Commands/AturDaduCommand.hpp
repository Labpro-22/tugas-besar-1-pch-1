#ifndef ATURDADUCOMMAND_HPP
#define ATURDADUCOMMAND_HPP

#include "Command.hpp"
#include <string>

// Forward declarations
class GameMaster;
class Player;
class Dice;

class AturDaduCommand : public Command {
private:
    GameMaster& gameMaster;
    Player* currentPlayer;
    Dice& dice;
    int targetVal1;
    int targetVal2;

public:
    // Konstruktor menerima referensi/pointer ke entitas yang dibutuhkan dan nilai dadu
    AturDaduCommand(GameMaster& gm, Player* player, Dice& d, int v1, int v2);

    // Override metode execute dari kelas induk Command
    void execute(GameMaster& gm) override;
};

#endif
