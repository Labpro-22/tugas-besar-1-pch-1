#ifndef FINANCIAL_EXCEPTIONS_HPP
#define FINANCIAL_EXCEPTIONS_HPP

#include "NimonspoliException.hpp"

class InsufficientFundsException : public NimonspoliException {
public:
    // Dipanggil saat uang pemain tidak cukup untuk Beli, Bayar Pajak, atau Tebus
    InsufficientFundsException(const std::string& playerName, const std::string& action) 
        : NimonspoliException("Error Finansial: Uang " + playerName + " tidak mencukupi untuk " + action + "!") {}
};

class InvalidBidException : public NimonspoliException {
public:
    // Dipanggil saat LelangCommand menerima bid yang lebih rendah dari current bid
    // atau jika penawar tidak memiliki uang sejumlah bid tersebut
    InvalidBidException() 
        : NimonspoliException("Error Lelang: Tawaran tidak valid! Pastikan nominal lebih tinggi dari harga saat ini dan uang Anda mencukupi.") {}
};

#endif