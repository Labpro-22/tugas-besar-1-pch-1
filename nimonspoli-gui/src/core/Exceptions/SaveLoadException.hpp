#ifndef SAVELOAD_EXCEPTION_HPP
#define SAVELOAD_EXCEPTION_HPP

#include "NimonsPoliException.hpp"

// Muat: File tidak ditemukan
class SaveFileNotFoundException : public NimonspoliException {
public:
    explicit SaveFileNotFoundException(const std::string& message)
        : NimonspoliException(message) {}
};

// Muat: Format isi file salah
class InvalidSaveFormatException : public NimonspoliException {
public:
    explicit InvalidSaveFormatException(const std::string& message)
        : NimonspoliException(message) {}
};

#endif