#ifndef FREE_FROM_JAIL_CARD_HPP
#define FREE_FROM_JAIL_CARD

#include "ChanceCard.hpp"
using namespace std;

class FreeFromJailCard : public ChanceCard
{
public:
    FreeFromJailCard();
    FreeFromJailCard(const string &type, const string &description);
    ~FreeFromJailCard();
    void execute(Player &p, GameState &gs) override;
};

#endif