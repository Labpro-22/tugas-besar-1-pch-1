#include "Property.hpp"

const int Property::INNER_WIDTH = 30;

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

int Property::getUsername() const
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
    status = newOwnerId.empty() ? PropertyStatus::BANK : PropertyStatus::OWNED;
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

void Property::printLine(ostringstream &out, char c)
{
    out << "+" << string(INNER_WIDTH + 2, c) << "+\n";
}

void Property::printRow(ostringstream &out, const string &leftText, const string &rightText)
{
    out << "| " << left << setw(18) << leftText
        << ": " << setw(10) << rightText << " |\n";
}

void Property::printFullRow(ostringstream &out, const string &text)
{
    string content = text;
    if ((int)content.size() > INNER_WIDTH)
        content = content.substr(0, INNER_WIDTH);

    out << "| " << left << setw(INNER_WIDTH) << content << " |\n";
}

void Property::printCenteredRow(ostringstream &out, const string &text)
{
    string content = text;
    if ((int)content.size() > INNER_WIDTH)
        content = content.substr(0, INNER_WIDTH);

    int totalPadding = INNER_WIDTH - (int)content.size();
    int leftPadding = totalPadding / 2;
    int rightPadding = totalPadding - leftPadding;

    out << "| "
        << string(leftPadding, ' ')
        << content
        << string(rightPadding, ' ')
        << " |\n";
}

void Property::printHeader(ostringstream &out) const
{
    printLine(out, '=');
    printCenteredRow(out, "AKTA KEPEMILIKAN");
    printCenteredRow(out, "[" + getColorGroup() + "] " +
                              getName() + " (" + getCode() + ")");
    printLine(out, '=');
}

void Property::printBasicInfo(ostringstream &out) const
{
    printRow(out, "Harga Beli", moneyToString(getPurchasePrice()));
    printRow(out, "Nilai Gadai", moneyToString(getMortageValue()));
}

void Property::printFooterStatus(ostringstream &out) const
{
    printLine(out, '=');

    string statusLine = statusToString(getStatus());
    if (!getOwnerId().empty())
        statusLine += " (" + getOwnerId() + ")";

    printFullRow(out, "Status : " + statusLine);
    printLine(out, '=');
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