#ifndef GUNAKAN_KARTU_KEMAMPUAN_COMMAND_HPP
#define GUNAKAN_KARTU_KEMAMPUAN_COMMAND_HPP

#include "Command.hpp"

class GunakanKartuKemampuanCommand : public Command
{
private:
    int cardIndex;

public:
    explicit GunakanKartuKemampuanCommand(int cardIndex);
    void execute(GameMaster &gm) override;
};

#endif