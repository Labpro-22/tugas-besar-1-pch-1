#ifndef ATURDADUCOMMAND_HPP
#define ATURDADUCOMMAND_HPP

#include "Command.hpp" // Asumsi ada base interface Command
#include <exception>
#include <string>

// Forward declaration
class Dice;

class AturDaduCommand : public Command {
private:
    Dice& dice; // Referensi ke objek Dice utama (Rule of Zero / Aggregation)
    int targetVal1;
    int targetVal2;

public:
    // Konstruktor menerima referensi dadu dan nilai yang ingin di-set (biasanya dari parser)
    AturDaduCommand(Dice& d, int v1, int v2);

    // Override metode execute dari kelas induk Command
    void execute(GameMaster& gm) override;
};

#endif