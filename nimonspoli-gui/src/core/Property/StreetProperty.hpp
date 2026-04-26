#ifndef STREET_PROPERTY_CPP
#define STREET_PROPERTY_CPP

#include "Property.hpp"
#include <map>

class StreetProperty : public Property
{
private:
    int houseUpgCost;
    int hotelUpgCost;
    map<int, int> rentPrice;
    int buildingCount;
    bool hasHotel;
    int festivalMultiplier;
    int festivalDuration;
    static constexpr int MAX_HOUSES = 4;
    static constexpr int MAX_FESTIVAL_MULTIPLIER = 8;
    static constexpr int FESTIVAL_DURATION = 3;
    int computeBaseRent(bool monopoly) const;
    int applyFestivalMultiplier(int rent) const;

public:
    StreetProperty();
    StreetProperty(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId, int houseUpgCost, int hotelUpgCost, map<int, int> rentPrice, int buildingCount, bool hasHotel, int festivalMultiplier, int festivalDuration);
    ~StreetProperty();

    int getHouseUpgCost() const;
    int getHotelUpgCost() const;
    const map<int, int> &getRentPrice() const;
    int getBuildingCount() const;
    bool gethasHotel() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;
    void setFestivalMultiplier(int multiplier);
    void setFestivalDuration(int duration);
    void buildHouse();
    void upgToHotel();
    int sellAllBuildings();
    void downgradeToHouse();
    void removeBuilding();
    void resetFestival();
    void activateFestival(int newMultiplier);
    void decrementFestivalDuration();
    int calculateRentPrice(int diceRoll,
                           int ownerSameColorCount,
                           bool monopoly) const override;
    int calculateSellPrice() const override;
    string cetakAkta() const override;
    string formattingTXT() const override;
    string printList() const override;
    void resetBuildings();
};

#endif