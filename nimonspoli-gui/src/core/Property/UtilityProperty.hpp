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
    UtilityProperty(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId, map<int, int> rentPrice);
    ~UtilityProperty();

    double calculateRentPrice();
    double calculateSellPrice();
    void formattingTXT() const override;
    friend ostream &operator<<(ostream &os, const UtilityProperty &p);
};

#endif