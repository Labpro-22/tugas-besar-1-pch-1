#ifndef PROPERTY_HPP
#define PROPERTY_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

enum class PropertyStatus
{
    BANK,
    OWNED,
    MORTGAGED
};

class Property
{
private:
    int id;
    string code;
    string name;
    string colorGroup;
    int purchasePrice;
    int mortageValue;
    PropertyStatus status;
    string ownerId;

protected:
    static string moneyToString(int value);
    static string statusToString(PropertyStatus status);

    static void printLine(ostringstream &out, char c = '=');
    static void printRow(ostringstream &out, const string &leftText, const string &rightText);
    static void printFullRow(ostringstream &out, const string &text);
    static void printCenteredRow(ostringstream &out, const string &text);

    // void printHeader(ostringstream &out) const;
    void printBasicInfo(ostringstream &out) const;
    void printFooterStatus(ostringstream &out) const;

public:
    Property();
    Property(int id, const string &code, const string &name, const string &colorGroup,
             int purchasePrice, int mortageValue, const string &ownerId);
    virtual ~Property();

    int getId() const;
    string getCode() const;
    string getName() const;
    string getColorGroup() const;
    int getPurchasePrice() const;
    int getMortageValue() const;
    PropertyStatus getStatus() const;
    string getOwnerId() const;

    void setOwner(const string &newOwnerId);
    void clearOwner();
    void setStatus(PropertyStatus newStatus);

    virtual int calculateRentPrice(int diceRoll,
                                   int ownerSameColorCount,
                                   bool monopoly) const = 0;
    virtual int calculateSellPrice() const = 0;
    virtual string cetakAkta() const = 0;
    virtual string formattingTXT() const = 0;

    virtual string printList() const = 0;
};

#endif