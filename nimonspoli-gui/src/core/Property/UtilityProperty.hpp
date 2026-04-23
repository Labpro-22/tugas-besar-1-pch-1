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

    const map<int, int> &getRentPrice() const;
    int calculateRentPrice(int diceRoll,
                           int ownerSameColorCount,
                           bool monopoly) const override;
    int calculateSellPrice() const override;
    string cetakAkta() const override;
    string formattingTXT() const override;
    string printList() const override;
};

#endif