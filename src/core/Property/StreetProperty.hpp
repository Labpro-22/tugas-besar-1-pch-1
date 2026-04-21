#ifndef STREET_PROPERTY_CPP
#define STREET_PROPERTY_CPP

#include "Property.hpp"
#include <map>

class StreetProperty : public Property
{
private:
    double houseUpgCost;
    double hotelUpgCost;
    map<int, int> rentPrice;
    int buildingCount;
    bool hasHotel;
    int festivalMultiplier;
    int festivalDuration;

public:
    StreetProperty();
    StreetProperty(const string &id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, PropertyStatus status, const string &ownerId, int houseUpgCost, int hotelUpgCost, map<int, int> rentPrice, int buildingCount, bool hasHotel, int festivalMultiplier, int festivalDuration);
    ~StreetProperty();

    int getHouseUpgCost() const;
    int getHotelUpgCost() const;
    int getBuildingCount() const;
    bool gethasHotel() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;
    void setFestivalMultiplier(int multiplier);
    void setFestivalDuration(int duration);
    void buildHouse();
    void upgToHotel();
    int sellAllBuildings();
    void resetFestival();
    int StreetProperty::computeBaseRent(bool monopoly) const;
    int applyFestivalMultiplier(int rent) const;
    int calculateRentPrice(int diceRoll,
                           int ownerSameColorCount,
                           bool monopoly) const override;
    int calculateSellPrice() const override;
    friend ostream &operator<<(ostream &os, const StreetProperty &p);
};

#endif