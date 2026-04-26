#include "Property.hpp"

Property::Property()
    : id(0), code(""), name(""), colorGroup(""),
      purchasePrice(0), mortageValue(0),
      status(PropertyStatus::BANK), ownerId("")
{
}

Property::Property(int id, const string &code, const string &name, const string &colorGroup,
                   int purchasePrice, int mortageValue, const string &ownerId)
    : id(id), code(code), name(name), colorGroup(colorGroup),
      purchasePrice(purchasePrice), mortageValue(mortageValue),
      ownerId(ownerId)
{
    this->status = ownerId.empty() ? PropertyStatus::BANK : PropertyStatus::OWNED;
}

Property::~Property()
{
}

int Property::getId() const
{
    return id;
}

string Property::getCode() const
{
    return code;
}

string Property::getName() const
{
    return name;
}

string Property::getColorGroup() const
{
    return colorGroup;
}

int Property::getPurchasePrice() const
{
    return purchasePrice;
}

int Property::getMortageValue() const
{
    return mortageValue;
}

PropertyStatus Property::getStatus() const
{
    return status;
}

string Property::getOwnerId() const
{
    return ownerId;
}

void Property::setOwner(const string &newOwnerId)
{
    ownerId = newOwnerId;
    if (newOwnerId.empty()) {
        status = PropertyStatus::BANK;
    } else if (status == PropertyStatus::MORTGAGED) {
        status = PropertyStatus::MORTGAGED;
    } else {
        status = PropertyStatus::OWNED;
    }
}

void Property::clearOwner()
{
    ownerId = "";
    status = PropertyStatus::BANK;
}

void Property::setStatus(PropertyStatus newStatus)
{
    status = newStatus;
}

string Property::moneyToString(int value)
{
    return "M" + to_string(value);
}

void Property::printLine(ostringstream &out, char ch)
{
    // Border ditangani GUI, jadi tidak mencetak apa pun.
}

void Property::printRow(ostringstream &out, const string &leftText, const string &rightText)
{
    out << leftText << " : " << rightText << "\n";
}

void Property::printFullRow(ostringstream &out, const string &text)
{
    out << text << "\n";
}

void Property::printCenteredRow(ostringstream &out, const string &text)
{
    out << text << "\n";
}
// void Property::printHeader(ostringstream &out) const
// {
//     // Header ditangani GUI.
// }

void Property::printBasicInfo(ostringstream &out) const
{
    printRow(out, "Harga Beli", moneyToString(purchasePrice));
    printRow(out, "Nilai Gadai", moneyToString(mortageValue));
    out << "\n";
}

void Property::printFooterStatus(ostringstream &out) const
{
    out << "\n";

    string statusLine = statusToString(status);

    if (!ownerId.empty() && ownerId != "BANK")
        statusLine += " (" + ownerId + ")";

    printRow(out, "Status", statusLine);
}

string Property::statusToString(PropertyStatus status)
{
    switch (status)
    {
    case PropertyStatus::BANK:
        return "BANK";
    case PropertyStatus::OWNED:
        return "OWNED";
    case PropertyStatus::MORTGAGED:
        return "MORTGAGED [M]";
    default:
        return "UNKNOWN";
    }
}