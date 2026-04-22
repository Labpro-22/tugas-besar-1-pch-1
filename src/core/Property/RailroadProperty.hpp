#ifndef RAILROAD_PROPERTY_CPP
#define RAILROAD_PROPERTY_CPP

#include "Property.hpp"
#include <iostream>
#include <map>

class RailroadProperty : public Property
{
private:
    map<int, int> rentFactor;

public:
    RailroadProperty();
    RailroadProperty(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId, map<int, int> rentFactor);
    ~RailroadProperty();

    int calculateRentPrice(int diceRoll,
                           int ownerSameColorCount,
                           bool monopoly) const override;
    int calculateSellPrice() const override;
    string formattingTXT() const override;
    friend ostream &operator<<(ostream &os, const RailroadProperty &p);
};

#endif