#ifndef GO_TO_JAIL_CARD_HPP
#define GO_TO_JAIL_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class GoToJailCard : public ChanceCard
{
public:
    GoToJailCard();
    ~GoToJailCard();
    void execute(Player &p, GameState &gs) override;
};

#endif