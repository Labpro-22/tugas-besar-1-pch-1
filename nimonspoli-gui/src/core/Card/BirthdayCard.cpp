#include "BirthdayCard.hpp"

BirthdayCard::BirthdayCard()
{
}

BirthdayCard::BirthdayCard(const string &type, const string &description, double amountPerPlayer) : GeneralFundCard(type, description), amountPerPlayer(amountPerPlayer)
{
}

BirthdayCard::~BirthdayCard()
{
}

void BirthdayCard::execute(Player &p, GameMaster &g)
{
}