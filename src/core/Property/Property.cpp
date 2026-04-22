#include "Property.hpp"

Property::Property()
{
}

Property::Property(int id, const string &code, const string &name, const string &colorGroup, int purchasePrice, int mortageValue, const string &ownerId) : id(id), code(code), name(name), colorGroup(colorGroup), purchasePrice(purchasePrice), mortageValue(mortageValue), ownerId(ownerId)
{
    this->status = PropertyStatus::BANK;
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
    status = PropertyStatus::OWNED;
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
