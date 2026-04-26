#ifndef DROP_KARTU_KEMAMPUAN_COMMAND_HPP
#define DROP_KARTU_KEMAMPUAN_COMMAND_HPP

#include "Command.hpp"

class DropKartuKemampuanCommand : public Command
{
private:
    int cardIndex; // 0-based index

public:
    explicit DropKartuKemampuanCommand(int cardIndex);
    void execute(GameMaster &gm) override;
};

#endif