#ifndef DROP_KARTU_KEMAMPUAN_COMMAND_HPP
#define DROP_KARTU_KEMAMPUAN_COMMAND_HPP

#include "Command.hpp"

class DropKartuKemampuanCommand : public Command
{
public:
    DropKartuKemampuanCommand() = default;
    void execute(GameMaster &gm) override;
};

#endif