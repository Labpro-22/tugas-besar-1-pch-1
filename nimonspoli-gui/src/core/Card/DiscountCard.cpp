#include "DiscountCard.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"

DiscountCard::DiscountCard()
    : SkillCard("SKILL", "Dapatkan diskon sewa acak selama 1 giliran.", false),
      discountPercent((rand() % 5 + 1) * 10.0),
      duration(1)
{
}

DiscountCard::DiscountCard(const string &type, const string &description, bool used, double discountPercent, int duration)
    : SkillCard(type, description, used),
      discountPercent(discountPercent),
      duration(duration)
{
}

DiscountCard::~DiscountCard()
{
}

int DiscountCard::getDuration()
{
    return duration;
}

double DiscountCard::getDiscountPercent() const
{
    return discountPercent;
}

void DiscountCard::decreaseDuration()
{
    duration--;
}

void DiscountCard::execute(Player &p, GameState &gs)
{
    p.setDiscount(discountPercent);
}