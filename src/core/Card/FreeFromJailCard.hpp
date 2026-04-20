#ifndef GO_TO_JAIL_CARD_HPP
#define GO_TO_JAIL_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class FreeFromJailCard : public ChanceCard
{
public:
    FreeFromJailCard();
    FreeFromJailCard(const string &type, const string &description);
    ~FreeFromJailCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif