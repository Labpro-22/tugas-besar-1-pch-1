#ifndef CETAK_AKTA_COMMAND_HPP
#define CETAK_AKTA_COMMAND_HPP

#include "Command.hpp"
#include <string>
using namespace std;

class GameState;

class CetakAktaCommand : public Command
{
private:
    string kodePetak;

public:
    explicit CetakAktaCommand(const string &kodePetak);
    void execute(GameMaster &gs) override;
};

#endif