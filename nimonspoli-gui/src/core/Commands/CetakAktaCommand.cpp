#include "CetakAktaCommand.hpp"

#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

using namespace std;

static string money(int value)
{
    return "M" + to_string(value);
}

static string statusToString(PropertyStatus status)
{
    switch (status)
    {
    case PropertyStatus::BANK:
        return "BANK";
    case PropertyStatus::OWNED:
        return "OWNED";
    case PropertyStatus::MORTGAGED:
        return "MORTGAGED";
    default:
        return "UNKNOWN";
    }
}

static void printLine(char c = '=')
{
    cout << "+" << string(32, c) << "+\n";
}

static void printRow(const string &leftText, const string &rightText)
{
    cout << "| " << left << setw(18) << leftText
         << ": " << setw(10) << rightText << " |\n";
}

// static void printFullRow(const string &text)
// {
//     cout << "| " << left << setw(30) << text << " |\n";
// }

static void printCenteredRow(const string &text)
{
    const int innerWidth = 30;

    string content = text;
    if ((int)content.size() > innerWidth)
    {
        content = content.substr(0, innerWidth);
    }

    int totalPadding = innerWidth - (int)content.size();
    int leftPadding = totalPadding / 2;
    int rightPadding = totalPadding - leftPadding;

    cout << "| "
         << string(leftPadding, ' ')
         << content
         << string(rightPadding, ' ')
         << " |\n";
}

CetakAktaCommand::CetakAktaCommand(const Property *property)
    : property(property)
{
}

void CetakAktaCommand::execute(GameMaster &gm)
{
    (void)gm;

    if (property == nullptr)
    {
        cout << "Petak tidak ditemukan atau bukan properti.\n";
        return;
    }

    string headerTitle = "[" + property->getColorGroup() + "] " +
                         property->getName() + " (" + property->getCode() + ")";

    if (const StreetProperty *sp = dynamic_cast<const StreetProperty *>(property))
    {
        printLine('=');
        printCenteredRow("AKTA KEPEMILIKAN");
        printCenteredRow(headerTitle);
        printLine('=');

        printRow("Harga Beli", money(sp->getPurchasePrice()));
        printRow("Nilai Gadai", money(sp->getMortageValue()));

        printLine('-');

        const map<int, int> &rentTable = sp->getRentPrice();
        for (const auto &[level, rent] : rentTable)
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

            printRow(label, money(rent));
        }

        printLine('-');

        printRow("Harga Rumah", money(sp->getHouseUpgCost()));
        printRow("Harga Hotel", money(sp->getHotelUpgCost()));

        printLine('=');

        string statusLine = statusToString(sp->getStatus());
        if (!sp->getOwnerId().empty())
        {
            statusLine += " (" + sp->getOwnerId() + ")";
        }
        printCenteredRow("Status : " + statusLine);
        printLine('=');
    }
    else if (const RailroadProperty *rp = dynamic_cast<const RailroadProperty *>(property))
    {
        printLine('=');
        printCenteredRow("AKTA KEPEMILIKAN");
        printCenteredRow(headerTitle);
        printLine('=');

        printRow("Harga Beli", money(rp->getPurchasePrice()));
        printRow("Nilai Gadai", money(rp->getMortageValue()));

        printLine('-');

        const map<int, int> &rentTable = rp->getRentFactor();
        for (const auto &[ownedCount, rent] : rentTable)
        {
            string label = "Sewa (" + to_string(ownedCount) + " railroad)";
            printRow(label, money(rent));
        }

        printLine('=');

        string statusLine = statusToString(rp->getStatus());
        if (!rp->getOwnerId().empty())
        {
            statusLine += " (" + rp->getOwnerId() + ")";
        }
        printCenteredRow("Status : " + statusLine);
        printLine('=');
    }
    else if (const UtilityProperty *up = dynamic_cast<const UtilityProperty *>(property))
    {
        printLine('=');
        printCenteredRow("AKTA KEPEMILIKAN");
        printCenteredRow(headerTitle);
        printLine('=');

        printRow("Harga Beli", money(up->getPurchasePrice()));
        printRow("Nilai Gadai", money(up->getMortageValue()));

        printLine('-');

        const map<int, int> &rentTable = up->getRentPrice();
        for (const auto &[ownedCount, multiplier] : rentTable)
        {
            string text = "Punya " + to_string(ownedCount) +
                          " utilitas: x" + to_string(multiplier);
            printCenteredRow(text);
        }

        printLine('=');

        string statusLine = statusToString(up->getStatus());
        if (!up->getOwnerId().empty())
        {
            statusLine += " (" + up->getOwnerId() + ")";
        }
        printCenteredRow("Status : " + statusLine);
        printLine('=');
    }
    else
    {
        cout << "Petak \"" << property->getCode()
             << "\" tidak ditemukan atau bukan properti.\n";
    }
}