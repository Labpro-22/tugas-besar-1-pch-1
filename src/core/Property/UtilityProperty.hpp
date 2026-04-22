#ifndef UTILITY_CPP
#define UTILITY_CPP

#include "Property.hpp"
#include <map>

using namespace std;

class UtilityProperty : public Property
{
private:
    map<int, int> rentPrice;

public:
    UtilityProperty();
    UtilityProperty(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId, map<int, int> rentPrice);
    ~UtilityProperty();

    int calculateRentPrice(int diceRoll,
                           int ownerSameColorCount,
                           bool monopoly) const override;
    int calculateSellPrice() const override;
    string formattingTXT() const override;
    friend ostream &operator<<(ostream &os, const UtilityProperty &p);
};

#endif