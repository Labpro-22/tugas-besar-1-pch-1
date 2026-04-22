#ifndef MOVE_BACK_THREE_CARD_HPP
#define MOVE_BACK_THREE_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class MoveToBackThreeCard : public ChanceCard
{
public:
    MoveToBackThreeCard();
    MoveToBackThreeCard(const string &type, const string &description);
    ~MoveToBackThreeCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif