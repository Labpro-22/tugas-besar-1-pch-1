#ifndef PROPERTY_CPP
#define PROPERTY_CPP

#include <string>
#include <iostream>
using namespace std;

enum class PropertyStatus
{
    BANK,
    OWNED,
    MORTGAGED
};

class Property
{
protected:
    string id;
    string code;
    string name;
    string colorGroup;
    double purchasePrice;
    double mortageValue;
    PropertyStatus status;
    string ownerId;

public:
    Property();
    Property(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId);
    virtual ~Property();
    string getId();
    string getCode();
    string getName();
    string getColorGroup();
    double getPurchasePrice();
    double getMortageValue();
    PropertyStatus getStatus();
    string getOwnerId();
    void setOwner(const string &newOwnerId);
    void clearOwner();
    void setStatus(PropertyStatus newStatus);
    double calculateRentalPrice();
    double calculateSellPrice();
    virtual void formattingTXT() const = 0;
    friend ostream &operator<<(ostream &os, const Property &p);
};

#endif