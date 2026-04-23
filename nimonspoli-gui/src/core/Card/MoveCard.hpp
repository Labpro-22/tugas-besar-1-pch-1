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
    MoveCard(int steps);
    ~MoveCard();
    void execute(Player &p, GameState &g) override;
    int getSteps() const;
};

#endif