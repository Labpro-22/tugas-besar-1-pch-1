#ifndef NEAREST_STATION_CARD_HPP
#define NEAREST_STATION_CARD_HPP

#include "ChanceCard.hpp"
using namespace std;

class NearestStationCard : public ChanceCard
{
public:
    NearestStationCard();
    NearestStationCard(const string &type, const string &description);
    ~NearestStationCard();
    void execute(Player &p, GameState &gs) override;
};

#endif