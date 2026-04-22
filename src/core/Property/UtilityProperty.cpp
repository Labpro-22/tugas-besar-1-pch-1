#include "UtilityProperty.hpp"

UtilityProperty::UtilityProperty()
{
}

UtilityProperty::UtilityProperty(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId, map<int, int> rentPrice) : Property(id, code, name, colorGroup, purchasePrice, mortageValue, ownerId), rentPrice(rentPrice)
{
}

UtilityProperty::~UtilityProperty()
{
}

int UtilityProperty::calculateRentPrice(int diceRoll,
                                        int ownerSameColorCount,
                                        bool monopoly) const
{
    return diceRoll * rentPrice.at(ownerSameColorCount);
}

int UtilityProperty::calculateSellPrice() const
{
    return getMortageValue();
}

ostream &operator<<(ostream &os, const UtilityProperty &p)
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

string UtilityProperty::formattingTXT() const
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

    return getCode() + " UTILITY " + ownerStr + " " + statusStr + " 1 0 0";
}