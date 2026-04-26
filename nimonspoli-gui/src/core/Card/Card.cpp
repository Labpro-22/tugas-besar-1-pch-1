#include "Card.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

Card::Card() : description("")
{
}

Card::Card(const string &description) : description(description)
{
}

Card::~Card()
{
}

void Card::execute(Player &p, GameState &gs)
{
}

string Card::getDescription()
{
    return description;
}