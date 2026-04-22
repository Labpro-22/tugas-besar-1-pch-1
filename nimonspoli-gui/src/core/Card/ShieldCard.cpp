#include "ShieldCard.hpp"

ShieldCard::ShieldCard()
{
}

ShieldCard::ShieldCard(const string &type, const string &description, bool used, int duration) : SkillCard(type, description, used), duration(duration)
{
}

ShieldCard::~ShieldCard()
{
}

int ShieldCard::getDuration()
{
    return duration;
}

void ShieldCard::decreaseDuration()
{
    duration = duration - 1;
}

void ShieldCard::execute(Player &p, GameMaster &g)
{
}