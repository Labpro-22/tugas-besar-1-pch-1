#include "Property.hpp"

Property::Property()
{
}

Property::Property(const string &id, const string &code, const string &name, const string &colorGroup, double purchasePrice, double mortageValue, PropertyStatus status, const string &ownerId) : id(id), code(code), name(name), colorGroup(colorGroup), purchasePrice(purchasePrice), mortageValue(mortageValue), status(status), ownerId(ownerId)
{
}

Property::~Property()
{
}

string Property::getId()
{
    return id;
}

string Property::getCode()
{
    return code;
}

string Property::getName()
{
    return name;
}

string Property::getColorGroup()
{
    return colorGroup;
}

double Property::getPurchasePrice()
{
    return purchasePrice;
}

double Property::getMortageValue()
{
    return mortageValue;
}

PropertyStatus Property::getStatus()
{
    return status;
}

string Property::getOwnerId()
{
    return ownerId;
}

void Property::setOwner(const string &newOwnerId)
{
    ownerId = newOwnerId;
}

void Property::clearOwner()
{
    ownerId = nullptr;
}

void Property::setStatus(PropertyStatus newStatus)
{
    status = newStatus;
}

double Property::calculateRentalPrice()
{
}

double Property::calculateSellPrice()
{
}
