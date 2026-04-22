#include "ChanceCard.hpp"

ChanceCard::ChanceCard()
{
}

ChanceCard::ChanceCard(const string &type, const string &description) : Card(type, description)
{
}

ChanceCard::~ChanceCard()
{
}

void ChanceCard::execute(Player &p, GameMaster &g)
{
}