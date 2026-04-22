#ifndef MOVE_CARD_HPP
#define MOVE_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class MoveCard : public SkillCard
{
private:
    int steps;

public:
    MoveCard();
    MoveCard(const string &type, const string &description, bool used, int steps);
    ~MoveCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif