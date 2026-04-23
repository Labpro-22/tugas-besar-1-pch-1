#include "StreetProperty.hpp"

StreetProperty::StreetProperty()
{
}

StreetProperty::StreetProperty(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId, int houseUpgCost, int hotelUpgCost, map<int, int> rentPrice, int buildingCount, bool hasHotel, int festivalMultiplier, int festivalDuration) : Property(id, code, name, colorGroup, purchasePrice, mortageValue, ownerId), houseUpgCost(houseUpgCost), hotelUpgCost(hotelUpgCost), rentPrice(rentPrice), buildingCount(buildingCount), hasHotel(hasHotel), festivalMultiplier(festivalMultiplier), festivalDuration(festivalDuration)
{
}

StreetProperty::~StreetProperty()
{
}

const map<int, int> &StreetProperty::getRentPrice() const
{
    return rentPrice;
}

int StreetProperty::getHouseUpgCost() const
{
    return houseUpgCost;
}

int StreetProperty::getHotelUpgCost() const
{
    return hotelUpgCost;
}

int StreetProperty::getBuildingCount() const
{
    return buildingCount;
}

bool StreetProperty::gethasHotel() const
{
    return hasHotel;
}

int StreetProperty::getFestivalMultiplier() const
{
    return festivalMultiplier;
}

int StreetProperty::getFestivalDuration() const
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
    if (!hasHotel && buildingCount < 4)
    {
        buildingCount++;
    }
}

void StreetProperty::upgToHotel()
{
    if (buildingCount == 4)
    {
        buildingCount = 0;
        hasHotel = true;
    }
}

int StreetProperty::sellAllBuildings()
{
    int value = 0;
    if (hasHotel)
    {
        value = hotelUpgCost / 2;
        hasHotel = false;
    }
    else
    {
        value = (buildingCount * houseUpgCost) / 2;
        buildingCount = 0;
    }
    return value;
}

void StreetProperty::resetFestival()
{
    festivalMultiplier = 1;
    festivalDuration = 0;
}

void StreetProperty::activateFestival(int newMultiplier)
{
    if (newMultiplier <= MAX_FESTIVAL_MULTIPLIER)
    {
        festivalMultiplier = newMultiplier;
    }
    festivalDuration = FESTIVAL_DURATION;
}

void StreetProperty::decrementFestivalDuration()
{
    if (festivalDuration > 0)
    {
        festivalDuration--;
        if (festivalDuration == 0)
        {
            festivalMultiplier = 1;
        }
    }
}

int StreetProperty::computeBaseRent(bool monopoly) const
{
    if (hasHotel)
        return rentPrice.at(5);
    if (buildingCount > 0)
        return rentPrice.at(buildingCount);
    if (monopoly)
        return rentPrice.at(0) * 2;
    return rentPrice.at(0);
}

int StreetProperty::applyFestivalMultiplier(int rent) const
{
    return rent * festivalMultiplier;
}

int StreetProperty::calculateRentPrice(int diceRoll,
                                       int ownerSameColorCount,
                                       bool monopoly) const
{
    int base = computeBaseRent(monopoly);
    return applyFestivalMultiplier(base);
}

int StreetProperty::calculateSellPrice() const
{
    int buildingValue = 0;
    if (hasHotel)
    {
        buildingValue = hotelUpgCost / 2;
    }
    else
    {
        buildingValue = (buildingCount * houseUpgCost) / 2;
    }
    return getPurchasePrice() + buildingValue;
}

string StreetProperty::formattingTXT() const
{
    string statusStr;
    switch (getStatus())
    {
    case PropertyStatus::BANK:
        statusStr = "BANK";
        break;
    case PropertyStatus::OWNED:
        statusStr = "OWNED";
        break;
    case PropertyStatus::MORTGAGED:
        statusStr = "MORTGAGED";
        break;
    }

    string ownerStr = getOwnerId().empty() ? "BANK" : getOwnerId();
    string bangunanStr = hasHotel ? "H" : to_string(buildingCount);

    return getCode() + " street " +
           ownerStr + " " +
           statusStr + " " +
           to_string(festivalMultiplier) + " " +
           to_string(festivalDuration) + " " +
           bangunanStr;
}

void StreetProperty::resetBuildings()
{
    buildingCount = 0;
    hasHotel = false;
}

string StreetProperty::cetakAkta() const
{
    ostringstream out;

    printHeader(out);
    printBasicInfo(out);

    printLine(out, '-');

    if (rentPrice.empty())
    {
        printFullRow(out, "Data sewa tidak tersedia");
    }
    else
    {
        for (const auto &[level, rent] : rentPrice)
        {
            string label;
            if (level == 0)
                label = "Sewa (unimproved)";
            else if (level >= 1 && level <= 4)
                label = "Sewa (" + to_string(level) + " rumah)";
            else if (level == 5)
                label = "Sewa (hotel)";
            else
                label = "Sewa (level " + to_string(level) + ")";

            printRow(out, label, moneyToString(rent));
        }
    }

    printLine(out, '-');
    printRow(out, "Harga Rumah", moneyToString(getHouseUpgCost()));
    printRow(out, "Harga Hotel", moneyToString(getHotelUpgCost()));

    printFooterStatus(out);

    return out.str();
}

string StreetProperty::printList() const
{
    ostringstream out;

    string namaKode = getName() + " (" + getCode() + ")";
    out << left << setw(26) << namaKode;

    string bangunan = "";
    if (hasHotel)
        bangunan = "Hotel";
    else if (buildingCount > 0)
        bangunan = to_string(buildingCount) + " rumah";

    out << setw(10) << bangunan;

    string harga = "M" + to_string(getPurchasePrice());
    out << setw(8) << harga;

    out << Property::statusToString(getStatus());

    return out.str();
}