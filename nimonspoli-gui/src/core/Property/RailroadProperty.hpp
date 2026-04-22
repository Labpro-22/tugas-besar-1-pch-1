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
    RailroadProperty(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId, map<int, int> rentFactor);
    ~RailroadProperty();

    double calculateRentPrice();
    double calculateSellPrice();
    void formattingTXT() const override;
    friend ostream &operator<<(ostream &os, const RailroadProperty &p);
};

#endif