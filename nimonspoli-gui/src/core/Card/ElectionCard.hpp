#ifndef ELECTION_CARD_HPP
#define ELECTION_CARD_HPP

#include "GeneralFundCard.hpp"
using namespace std;

class ElectionCard : public GeneralFundCard
{
private:
    double amountPerPlayer;

public:
    ElectionCard();
    ElectionCard(const string &type, const string &description, double amountPerPlayer);
    ~ElectionCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif