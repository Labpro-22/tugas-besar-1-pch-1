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
    ShieldCard(const string &type, const string &description, bool used, int duration);
    ~ShieldCard();
    int getDuration();
    void decreaseDuration();
    void execute(Player &p, GameMaster &g) override;
};

#endif