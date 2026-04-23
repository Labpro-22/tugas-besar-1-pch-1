#include "CetakPropertiCommand.hpp"

#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

using namespace std;

static const int INNER_WIDTH = 30;

static string moneyToString(int value)
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
    cout << "+" << string(INNER_WIDTH + 2, c) << "+\n";
}

static void printRow(const string &leftText, const string &rightText)
{
    cout << "| " << left << setw(18) << leftText
         << ": " << setw(10) << rightText << " |\n";
}

static void printFullRow(const string &text)
{
    string content = text;
    if ((int)content.size() > INNER_WIDTH)
    {
        content = content.substr(0, INNER_WIDTH);
    }

    cout << "| " << left << setw(INNER_WIDTH) << content << " |\n";
}

static void printCenteredRow(const string &text)
{
    string content = text;
    if ((int)content.size() > INNER_WIDTH)
    {
        content = content.substr(0, INNER_WIDTH);
    }

    int totalPadding = INNER_WIDTH - (int)content.size();
    int leftPadding = totalPadding / 2;
    int rightPadding = totalPadding - leftPadding;

    cout << "| "
         << string(leftPadding, ' ')
         << content
         << string(rightPadding, ' ')
         << " |\n";
}

static string makeHeaderTitle(const Property *property)
{
    if (dynamic_cast<const StreetProperty *>(property))
    {
        return "[" + property->getColorGroup() + "] " +
               property->getName() + " (" + property->getCode() + ")";
    }

    return property->getName() + " (" + property->getCode() + ")";
}

CetakPropertiCommand::CetakPropertiCommand(const Property *property)
    : property(property)
{
}

void CetakPropertiCommand::execute(GameMaster &gm)
{
    (void)gm;

    if (property == nullptr)
    {
        cout << "Petak tidak ditemukan atau bukan properti.\n";
        return;
    }

    string headerTitle = makeHeaderTitle(property);

    if (const StreetProperty *sp = dynamic_cast<const StreetProperty *>(property))
    {
        printLine('=');
        printCenteredRow("AKTA KEPEMILIKAN");
        printCenteredRow(headerTitle);
        printLine('=');

        printRow("Harga Beli", moneyToString(sp->getPurchasePrice()));
        printRow("Nilai Gadai", moneyToString(sp->getMortageValue()));

        printLine('-');

        const map<int, int> &rentTable = sp->getRentPrice();
        if (rentTable.empty())
        {
            printFullRow("Data sewa tidak tersedia");
        }
        else
        {
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

                printRow(label, moneyToString(rent));
            }
        }

        printLine('-');

        printRow("Harga Rumah", moneyToString(sp->getHouseUpgCost()));
        printRow("Harga Hotel", moneyToString(sp->getHotelUpgCost()));

        printLine('=');

        string statusLine = statusToString(sp->getStatus());
        if (!sp->getOwnerId().empty())
        {
            statusLine += " (" + sp->getOwnerId() + ")";
        }

        printFullRow("Status : " + statusLine);
        printLine('=');
    }
    else if (const RailroadProperty *rp = dynamic_cast<const RailroadProperty *>(property))
    {
        printLine('=');
        printCenteredRow("AKTA KEPEMILIKAN");
        printCenteredRow(headerTitle);
        printLine('=');

        printRow("Harga Beli", moneyToString(rp->getPurchasePrice()));
        printRow("Nilai Gadai", moneyToString(rp->getMortageValue()));

        printLine('-');

        const map<int, int> &rentTable = rp->getRentFactor();
        if (rentTable.empty())
        {
            printFullRow("Data sewa tidak tersedia");
        }
        else
        {
            for (const auto &[ownedCount, rent] : rentTable)
            {
                string label = "Sewa (" + to_string(ownedCount) + " railroad)";
                printRow(label, moneyToString(rent));
            }
        }

        printLine('=');

        string statusLine = statusToString(rp->getStatus());
        if (!rp->getOwnerId().empty())
        {
            statusLine += " (" + rp->getOwnerId() + ")";
        }

        printFullRow("Status : " + statusLine);
        printLine('=');
    }
    else if (const UtilityProperty *up = dynamic_cast<const UtilityProperty *>(property))
    {
        printLine('=');
        printCenteredRow("AKTA KEPEMILIKAN");
        printCenteredRow(headerTitle);
        printLine('=');

        printRow("Harga Beli", moneyToString(up->getPurchasePrice()));
        printRow("Nilai Gadai", moneyToString(up->getMortageValue()));

        printLine('-');

        const map<int, int> &rentTable = up->getRentPrice();
        if (rentTable.empty())
        {
            printFullRow("Data sewa tidak tersedia");
        }
        else
        {
            for (const auto &[ownedCount, multiplier] : rentTable)
            {
                string text = "Punya " + to_string(ownedCount) +
                              " utilitas: x" + to_string(multiplier);
                printFullRow(text);
            }
        }

        printLine('=');

        string statusLine = statusToString(up->getStatus());
        if (!up->getOwnerId().empty())
        {
            statusLine += " (" + up->getOwnerId() + ")";
        }

        printFullRow("Status : " + statusLine);
        printLine('=');
    }
    else
    {
        cout << "Petak \"" << property->getCode()
             << "\" tidak ditemukan atau bukan properti.\n";
    }
}