#ifndef SHIELD_CARD_HPP
#define SHIELD_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class ShieldCard : public SkillCard
{
private:
    int duration;

public:
    ShieldCard();
    ~ShieldCard();
    int getDuration() const;
    void decreaseDuration();
    void execute(Player &p, GameState &gs) override;
};

#endif