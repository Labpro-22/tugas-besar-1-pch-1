#include "LassoCard.hpp"

LassoCard::LassoCard()
{
}

LassoCard::LassoCard(const string &type, const string &description, bool used) : SkillCard(type, description, used)
{
}

LassoCard::~LassoCard()
{
}

void LassoCard::execute(Player &p, GameMaster &g)
{
}