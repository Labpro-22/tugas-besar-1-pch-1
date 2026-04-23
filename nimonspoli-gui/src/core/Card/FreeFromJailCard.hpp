#ifndef FREE_FROM_JAIL_CARD_HPP
#define FREE_FROM_JAIL_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class FreeFromJailCard : public ChanceCard
{
public:
    FreeFromJailCard();
    ~FreeFromJailCard();
    void execute(Player &p, GameState &gs) override;
};

#endif