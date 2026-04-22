#include "StreetProperty.hpp"

StreetProperty::StreetProperty()
{
}

StreetProperty::StreetProperty(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId, double houseUpgCost, double hotelUpgCost, map<int, int> rentPrice, int buildingCount, bool hasHotel, int festivalMultiplier, int festivalDuration) : Property(id, code, name, colorGroup, purchasePrice, mortageValue, status, ownerId), houseUpgCost(houseUpgCost), hotelUpgCost(hotelUpgCost), rentPrice(rentPrice), buildingCount(buildingCount), hasHotel(hasHotel), festivalMultiplier(festivalMultiplier), festivalDuration(festivalDuration)
{
}

StreetProperty::~StreetProperty()
{
}

double StreetProperty::getHouseUpgCost()
{
    return houseUpgCost;
}

double StreetProperty::getHotelUpgCost()
{
    return hotelUpgCost;
}

int StreetProperty::getBuildingCount()
{
    return buildingCount;
}

bool StreetProperty::gethasHotel()
{
    return hasHotel;
}

int StreetProperty::getFestivalMultiplier()
{
    return festivalMultiplier;
}

int StreetProperty::getFestivalDuration()
{
    return festivalDuration;
}

void StreetProperty::setFestivalMultiplier(int multiplier)
{
    festivalMultiplier = multiplier;
}

void StreetProperty::setFestivalDuration(int duration)
{
    festivalDuration = duration;
}

void StreetProperty::buildHouse()
{
}

void StreetProperty::upgToHotel()
{
}

double StreetProperty::sellAllBuildings()
{
}

void StreetProperty::resetFestival()
{
}

double StreetProperty::calculateSellPrice()
{
}

double StreetProperty::calculateRentPrice()
{
}

ostream &operator<<(ostream &os, const StreetProperty &p)
{
}
