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
    StreetProperty(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId, double houseUpgCost, double hotelUpgCost, map<int, int> rentPrice, int buildingCount, bool hasHotel, int festivalMultiplier, int festivalDuration);
    ~StreetProperty();

    double getHouseUpgCost();
    double getHotelUpgCost();
    int getBuildingCount();
    bool gethasHotel();
    int getFestivalMultiplier();
    int getFestivalDuration();
    void setFestivalMultiplier(int multiplier);
    void setFestivalDuration(int duration);
    void buildHouse();
    void upgToHotel();
    double sellAllBuildings();
    void resetFestival();
    double calculateSellPrice();
    double calculateRentPrice();
    friend ostream &operator<<(ostream &os, const StreetProperty &p);
};

#endif