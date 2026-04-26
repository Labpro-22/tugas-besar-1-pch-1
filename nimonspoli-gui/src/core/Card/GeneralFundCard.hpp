#ifndef GENERAL_FUND_CARD_HPP
#define GENERAL_FUND_CARD_HPP

#include "Card.hpp"
using namespace std;

class GeneralFundCard : public Card
{
public:
    GeneralFundCard();
    GeneralFundCard(const string &description);
    virtual ~GeneralFundCard();
    virtual void execute(Player &p, GameState &gs) = 0;
};

#endif