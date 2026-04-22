#ifndef CHANCE_CARD_HPP
#define CHANCE_CARD_HPP

#include "Card.hpp"
using namespace std;

class ChanceCard : public Card
{
public:
    ChanceCard();
    ChanceCard(const string &type, const string &description);
    virtual ~ChanceCard();
    virtual void execute(Player &p, GameMaster &g) = 0;
};

#endif