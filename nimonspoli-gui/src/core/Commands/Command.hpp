#ifndef COMMAND_HPP
#define COMMAND_HPP

class GameMaster;

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(GameMaster& gm) = 0;
};

#endif