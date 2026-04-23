#ifndef ELECTION_CARD_HPP
#define ELECTION_CARD_HPP

#include "GeneralFundCard.hpp"
using namespace std;

class ElectionCard : public GeneralFundCard
{
private:
    int amountPerPlayer;

public:
    ElectionCard();
    ~ElectionCard();
    int getAmountPerPlayer() const;
    void execute(Player &p, GameState &gs) override;
};

#endif