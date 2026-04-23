#ifndef CETAK_PROPERTI_COMMAND_HPP
#define CETAK_PROPERTI_COMMAND_HPP

#include "Command.hpp"
#include "../Property/Property.hpp"

class CetakPropertiCommand : public Command
{
private:
    const Property *property;

public:
    explicit CetakPropertiCommand(const Property *property);
    void execute(GameMaster &gm) override;
};

#endif