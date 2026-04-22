#include "DiscountCard.hpp"

DiscountCard::DiscountCard()
{
}

DiscountCard::DiscountCard(const string &type, const string &description, bool used, double discountPercent, int duration) : SkillCard(type, description, used), discountPercent(discountPercent), duration(duration)
{
}

DiscountCard::~DiscountCard()
{
}

int DiscountCard::getDuration()
{
    return duration;
}

void DiscountCard::decreaseDuration()
{
    duration -= 1;
}

void DiscountCard::execute(Player &p, GameMaster &g)
{
}