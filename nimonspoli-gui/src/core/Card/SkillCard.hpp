#ifndef SKILL_CARD_HPP
#define SKILL_CARD_HPP

#include "Card.hpp"
using namespace std;

class SkillCard : public Card
{
private:
    bool used;

public:
    SkillCard();
    SkillCard(const string &type, const string &description, bool used);
    virtual ~SkillCard();
    bool getUsed();
    void setUsed(bool newUsed);
    virtual void execute(Player &p, GameMaster &g) = 0;
};

#endif