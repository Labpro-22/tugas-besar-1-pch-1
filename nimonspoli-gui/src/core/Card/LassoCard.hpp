#ifndef LASSO_CARD_HPP
#define LASSO_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class LassoCard : public SkillCard
{
public:
    LassoCard();
    LassoCard(const string &type, const string &description, bool used);
    ~LassoCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif