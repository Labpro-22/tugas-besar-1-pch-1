#include "DiscountCard.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"

DiscountCard::DiscountCard() : DiscountCard(rand() % 50 + 10, 1) {}

DiscountCard::DiscountCard(int discountPercent, int duration) : SkillCard("Diskon " + to_string(discountPercent) + "%", "DiscountCard"), discountPercent(discountPercent), duration(duration) {}

DiscountCard::DiscountCard(const string &type, const string &description, bool used, double discountPercent, int duration) : SkillCard(type, description, used), discountPercent(static_cast<int>(discountPercent)), duration(duration) {}

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

string DiscountCard::successMessage() const
{
    return "DiscountCard diaktifkan. Anda mendapat diskon " +
           to_string(getDiscountPercent()) +
           "% selama " +
           to_string(getDuration()) +
           " giliran.";
}