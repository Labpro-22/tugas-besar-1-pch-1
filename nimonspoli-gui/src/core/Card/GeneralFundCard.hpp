#ifndef GENERAL_FUND_CARD_HPP
#define GENERAL_FUND_CARD_HPP

#include "Card.hpp"
using namespace std;

class GeneralFundCard : public Card
{
public:
    GeneralFundCard();
    GeneralFundCard(const string &type, const string &description);
    virtual ~GeneralFundCard();
    virtual void execute(Player &p, GameMaster &g) = 0;
};

#endif