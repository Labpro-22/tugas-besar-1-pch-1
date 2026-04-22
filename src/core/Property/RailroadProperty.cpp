#include "RailroadProperty.hpp"

RailroadProperty::RailroadProperty()
{
}

RailroadProperty::RailroadProperty(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId, map<int, int> rentFactor) : Property(id, code, name, colorGroup, purchasePrice, mortageValue, ownerId), rentFactor(rentFactor)
{
}

RailroadProperty::~RailroadProperty()
{
}

int RailroadProperty::calculateRentPrice(int diceRoll,
                                         int ownerSameColorCount,
                                         bool monopoly) const
{
    return rentFactor.at(ownerSameColorCount);
}

int RailroadProperty::calculateSellPrice() const
{
    return getMortageValue();
}

ostream &operator<<(ostream &os, const RailroadProperty &p)
{
    string namaKode = p.getName() + " (" + p.getCode() + ")";
    os << left << setw(26) << namaKode;

    os << setw(10) << "";

    string harga = "M" + to_string(p.getMortageValue() * 2);
    os << setw(8) << harga;

    if (p.getStatus() == PropertyStatus::MORTGAGED)
        os << "MORTGAGED [M]";
    else
        os << "OWNED";
    return os;
}

string RailroadProperty::formattingTXT() const
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

    return getCode() + " RAILROAD " + ownerStr + " " + statusStr + " 1 0 0";
}