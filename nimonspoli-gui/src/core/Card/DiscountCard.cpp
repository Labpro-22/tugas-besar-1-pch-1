#include "DiscountCard.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"

DiscountCard::DiscountCard() : DiscountCard(rand() % 50 + 10, 1) {}

DiscountCard::DiscountCard(int discountPercent, int duration) : SkillCard("Diskon " + to_string(discountPercent) + "%", "DiscountCard"), discountPercent(discountPercent), duration(duration) {}

DiscountCard::~DiscountCard()
{
}

int DiscountCard::getDuration() const
{
    return duration;
}

void DiscountCard::decreaseDuration()
{
    duration--;
}

int DiscountCard::getDiscountPercent() const
{
    return discountPercent;
}

void DiscountCard::execute(Player &p, GameState &gs)
{
    p.setDiscount(discountPercent);
}