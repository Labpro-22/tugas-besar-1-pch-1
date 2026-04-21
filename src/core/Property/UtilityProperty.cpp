#include "UtilityProperty.hpp"

UtilityProperty::UtilityProperty()
{
}

UtilityProperty::UtilityProperty(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId, map<int, int> rentPrice) : Property(id, code, name, colorGroup, purchasePrice, mortageValue, status, ownerId), rentPrice(rentPrice)
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
