#include "TeleportCard.hpp"

TeleportCard::TeleportCard()
{
}

TeleportCard::TeleportCard(const string &type, const string &description, bool used) : SkillCard(type, description, used)
{
}

TeleportCard::~TeleportCard()
{
}

void TeleportCard::execute(Player &p, GameMaster &g)
{
}