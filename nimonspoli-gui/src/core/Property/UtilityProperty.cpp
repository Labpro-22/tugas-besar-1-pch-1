#include "UtilityProperty.hpp"
#include <iostream>

UtilityProperty::UtilityProperty() {}

UtilityProperty::UtilityProperty(
    const string& id, const string& code, const string& name,
    const string& colorGroup, double purchasePrice, double mortageValue,
    PropertyStatus status, const string& ownerId, map<int,int> rentPrice)
    : Property(id, code, name, colorGroup, purchasePrice, mortageValue, status, ownerId),
      rentPrice(rentPrice) {}

UtilityProperty::~UtilityProperty() {}

double UtilityProperty::calculateRentPrice() {
    // Sewa = diceTotal × faktor pengali (tergantung jumlah utility dimiliki)
    // diceTotal di-inject dari UtilityTile::calculateRent(diceTotal)
    // Di sini return faktor pengali saja; perkalian di UtilityTile
    return 1;
}

double UtilityProperty::calculateSellPrice() {
    return purchasePrice;
}

void UtilityProperty::formattingTXT() const {
    // TODO: implementasi serialisasi untuk Save/Load
}

ostream& operator<<(ostream& os, const UtilityProperty& p) {
    os << "[UTILITAS] " << p.name << " (" << p.code << ")"
       << " | Pemilik: " << (p.ownerId.empty() ? "BANK" : p.ownerId)
       << " | Status: " << (p.status == PropertyStatus::MORTGAGED ? "MORTGAGED" : "OWNED");
    return os;
}