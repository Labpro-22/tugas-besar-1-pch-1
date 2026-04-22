#ifndef GO_TO_JAIL_CARD_HPP
#define GO_TO_JAIL_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class GoToJailCard : public ChanceCard
{
public:
    GoToJailCard();
    GoToJailCard(const string &type, const string &description);
    ~GoToJailCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif