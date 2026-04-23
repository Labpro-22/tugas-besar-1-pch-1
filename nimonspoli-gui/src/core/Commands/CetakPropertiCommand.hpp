#ifndef CETAK_PROPERTI_COMMAND_HPP
#define CETAK_PROPERTI_COMMAND_HPP

#include "Command.hpp"

class CetakPropertiCommand : public Command
{
public:
    CetakPropertiCommand() = default;
    void execute(GameMaster &gm) override;
};

#endif