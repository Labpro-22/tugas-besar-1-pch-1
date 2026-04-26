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

const map<int, int> &UtilityProperty::getRentPrice() const
{
    return rentPrice;
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

string UtilityProperty::cetakAkta() const
{
    ostringstream out;

    // printHeader(out);
    printBasicInfo(out);

    printLine(out, '-');

    if (rentPrice.empty())
    {
        printFullRow(out, "Data sewa tidak tersedia");
    }
    else
    {
        for (const auto &[ownedCount, multiplier] : rentPrice)
        {
            printFullRow(out,
                         "Punya " + to_string(ownedCount) +
                             " utilitas: x" + to_string(multiplier));
        }
    }

    printFooterStatus(out);

    return out.str();
}

string UtilityProperty::printList() const
{
    ostringstream out;

    string namaKode = getName() + " (" + getCode() + ")";
    out << left << setw(26) << namaKode;
    out << setw(10) << "";

    string harga = "M" + to_string(getPurchasePrice());
    out << setw(8) << harga;

    out << Property::statusToString(getStatus());

    return out.str();
}