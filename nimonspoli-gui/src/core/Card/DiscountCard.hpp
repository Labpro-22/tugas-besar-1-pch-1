#ifndef DISCOUNT_CARD_HPP
#define DISCOUNT_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class DiscountCard : public SkillCard
{
private:
    double discountPercent;
    int duration;

public:
    DiscountCard();
    DiscountCard(const string &type, const string &description, bool used, double discountPercent, int duration);
    ~DiscountCard();
    int getDuration();
    void decreaseDuration();
    void execute(Player &p, GameMaster &g) override;
};

#endif