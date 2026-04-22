#include "ElectionCard.hpp"

ElectionCard::ElectionCard()
{
}

ElectionCard::ElectionCard(const string &type, const string &description, double amountPerPlayer) : GeneralFundCard(type, description), amountPerPlayer(amountPerPlayer)
{
}

ElectionCard::~ElectionCard()
{
}

void ElectionCard::execute(Player &p, GameMaster &g)
{
}