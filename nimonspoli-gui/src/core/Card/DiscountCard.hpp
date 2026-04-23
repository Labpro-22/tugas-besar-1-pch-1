#ifndef DISCOUNT_CARD_HPP
#define DISCOUNT_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class DiscountCard : public SkillCard
{
private:
    int discountPercent;
    int duration;

public:
    DiscountCard();
    DiscountCard(int discountPercent, int duration);
    ~DiscountCard();
    int getDuration() const;
    void decreaseDuration();
    int getDiscountPercent() const;
    void execute(Player &p, GameState &gs) override;
};

#endif