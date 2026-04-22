#include "Card.hpp"

Card::Card()
{
}

Card::Card(const string &type, const string &description) : type(type), description(description)
{
}

Card::~Card()
{
}

void Card::execute(Player &p, GameMaster &g)
{
}

string Card::getDescription()
{
    return description;
}

string Card::getType()
{
    return type;
}