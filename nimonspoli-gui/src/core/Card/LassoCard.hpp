#ifndef LASSO_CARD_HPP
#define LASSO_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class LassoCard : public SkillCard
{
public:
    LassoCard();
    ~LassoCard();
    void execute(Player &p, GameState &gs) override;
};

#endif