#ifndef BIRTHDAY_CARD_HPP
#define BIRTHDAY_CARD_HPP

#include "GeneralFundCard.hpp"
using namespace std;

class BirthdayCard : public GeneralFundCard
{
private:
    double amountPerPlayer;

public:
    BirthdayCard();
    BirthdayCard(const string &type, const string &description, double amountPerPlayer);
    ~BirthdayCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif