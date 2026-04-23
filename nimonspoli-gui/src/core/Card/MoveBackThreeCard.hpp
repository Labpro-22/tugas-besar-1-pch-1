#ifndef MOVE_BACK_THREE_CARD_HPP
#define MOVE_BACK_THREE_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class MoveToBackThreeCard : public ChanceCard
{
public:
    MoveToBackThreeCard();
    ~MoveToBackThreeCard();
    void execute(Player &p, GameState &gs) override;
};

#endif