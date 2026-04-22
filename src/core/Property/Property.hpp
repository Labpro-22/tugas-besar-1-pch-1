#ifndef PROPERTY_CPP
#define PROPERTY_CPP

#include <string>
#include <iostream>
#include <iomanip>
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

public:
    Property();
    Property(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId);
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
    virtual string formattingTXT() const = 0;
    friend ostream &operator<<(ostream &os, const Property &p);
};

#endif