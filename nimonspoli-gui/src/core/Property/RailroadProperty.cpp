#include "RailroadProperty.hpp"
#include <iostream>

RailroadProperty::RailroadProperty() {}

RailroadProperty::RailroadProperty(
    const string& id, const string& code, const string& name,
    const string& colorGroup, double purchasePrice, double mortageValue,
    PropertyStatus status, const string& ownerId, map<int,int> rentFactor)
    : Property(id, code, name, colorGroup, purchasePrice, mortageValue, status, ownerId),
      rentFactor(rentFactor) {}

RailroadProperty::~RailroadProperty() {}

double RailroadProperty::calculateRentPrice() {
    // Dihitung berdasarkan jumlah railroad yang dimiliki pemilik
    // Jumlah railroad diketahui dari GameMaster::countPlayerRailroads()
    // dan di-inject saat pemanggilan — nilai default 0 jika belum diset
    return 0;
}

double RailroadProperty::calculateSellPrice() {
    return purchasePrice;
}

void RailroadProperty::formattingTXT() const {
    // TODO: implementasi serialisasi untuk Save/Load
    // Format: KODE JENIS PEMILIK STATUS FMULT FDUR N_BANGUNAN
}

ostream& operator<<(ostream& os, const RailroadProperty& p) {
    os << "[STASIUN] " << p.name << " (" << p.code << ")"
       << " | Pemilik: " << (p.ownerId.empty() ? "BANK" : p.ownerId)
       << " | Status: " << (p.status == PropertyStatus::MORTGAGED ? "MORTGAGED" : "OWNED");
    return os;
}