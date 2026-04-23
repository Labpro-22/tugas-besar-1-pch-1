#ifndef BIRTHDAY_CARD_HPP
#define BIRTHDAY_CARD_HPP

#include "GeneralFundCard.hpp"
using namespace std;

class BirthdayCard : public GeneralFundCard
{
private:
    int amountPerPlayer;

public:
    BirthdayCard();
    ~BirthdayCard();
    int getAmountPerPlayer() const;
    void execute(Player &p, GameState &gs) override;
};

#endif