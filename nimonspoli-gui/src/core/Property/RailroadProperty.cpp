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

const map<int, int> &RailroadProperty::getRentFactor() const
{
    return rentFactor;
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

string RailroadProperty::cetakAkta() const
{
    ostringstream out;

    // printHeader(out);
    printBasicInfo(out);

    printLine(out, '-');

    if (rentFactor.empty())
    {
        printFullRow(out, "Data sewa tidak tersedia");
    }
    else
    {
        for (const auto &[ownedCount, rent] : rentFactor)
        {
            string label = "Sewa (" + to_string(ownedCount) + " railroad)";
            printRow(out, label, moneyToString(rent));
        }
    }

    printFooterStatus(out);

    return out.str();
}

string RailroadProperty::printList() const
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