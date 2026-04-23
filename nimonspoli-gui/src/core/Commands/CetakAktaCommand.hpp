#ifndef CETAK_AKTA_COMMAND_HPP
#define CETAK_AKTA_COMMAND_HPP

#include "Command.hpp"
#include "../Property/Property.hpp"

class CetakAktaCommand : public Command
{
private:
    const Property *property;

public:
    explicit CetakAktaCommand(const Property *property);
    void execute(GameMaster &gm) override;
};

#endif