#ifndef SKILL_CARD_HPP
#define SKILL_CARD_HPP

#include "Card.hpp"
using namespace std;

class SkillCard : public Card
{
private:
    string type;
    bool used;

public:
    SkillCard();
    SkillCard(const string &description, const string &type);
    virtual ~SkillCard();
    bool isUsed() const;
    void markUsed();
    string getType();
    virtual void execute(Player &p, GameState &gs) = 0;
};

#endif