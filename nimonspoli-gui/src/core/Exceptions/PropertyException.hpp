#ifndef PROPERTY_EXCEPTIONS_HPP
#define PROPERTY_EXCEPTIONS_HPP

#include "NimonspoliException.hpp"

class PropertyAlreadyOwnedException : public NimonspoliException {
public:
    // Dipanggil saat BeliCommand mencoba membeli tanah yang sudah ada pemiliknya
    PropertyAlreadyOwnedException(const std::string& propName) 
        : NimonspoliException("Error Properti: " + propName + " sudah memiliki pemilik!") {}
};

class NotPropertyOwnerException : public NimonspoliException {
public:
    // Dipanggil saat GadaiCommand atau TebusCommand dieksekusi oleh pemain yang bukan pemilik properti
    NotPropertyOwnerException(const std::string& propName) 
        : NimonspoliException("Error Properti: Anda tidak berhak melakukan aksi ini karena bukan pemilik dari " + propName + "!") {}
};

class InvalidMortgageStateException : public NimonspoliException {
public:
    // Dipanggil saat mencoba menggadai properti yang sudah tergadai, 
    // atau menebus properti yang tidak sedang tergadai
    InvalidMortgageStateException(const std::string& propName, const std::string& expectedState) 
        : NimonspoliException("Error Gadai: Status " + propName + " tidak valid. Properti " + expectedState + ".") {}
};

class PropertyHasBuildingsException : public NimonspoliException {
public:
    // Dipanggil saat GadaiCommand dieksekusi pada tanah yang masih memiliki rumah/hotel
    PropertyHasBuildingsException(const std::string& propName) 
        : NimonspoliException("Error Properti: " + propName + " masih memiliki bangunan! Jual bangunan terlebih dahulu sebelum menggadai tanah.") {}
};

#endif