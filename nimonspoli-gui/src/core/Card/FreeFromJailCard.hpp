#ifndef FREE_FROM_JAIL_CARD_HPP
#define FREE_FROM_JAIL_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class FreeFromJailCard : public SkillCard
{
public:
    FreeFromJailCard();
    FreeFromJailCard(const string &type, const string &description, bool used);
    ~FreeFromJailCard();

    void execute(Player &p, GameState &gs) override;
    string successMessage() const override;
};

#endif