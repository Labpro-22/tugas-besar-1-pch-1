#ifndef LASSO_CARD_HPP
#define LASSO_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class LassoCard : public SkillCard
{
private:
    string targetPlayerUsername;

public:
    LassoCard();
    LassoCard(const string &type, const string &description, bool used);
    ~LassoCard();
    void setTargetPlayerUsername(const string &target);
    void execute(Player &p, GameState &gs) override;
    string successMessage() const override;
};

#endif