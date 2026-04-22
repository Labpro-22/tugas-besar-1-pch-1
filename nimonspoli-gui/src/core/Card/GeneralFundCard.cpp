#include "GeneralFundCard.hpp"

GeneralFundCard::GeneralFundCard()
{
}

GeneralFundCard::GeneralFundCard(const string &type, const string &description) : Card(type, description)
{
}

GeneralFundCard::~GeneralFundCard()
{
}

void GeneralFundCard::execute(Player &p, GameMaster &g)
{
}