#ifndef DICE_EXCEPTIONS_HPP
#define DICE_EXCEPTIONS_HPP

#include "NimonspoliException.hpp"

class InvalidDiceValueException : public NimonspoliException {
public:
    // Dipanggil saat AturDaduCommand menerima input di luar 1-6
    InvalidDiceValueException(int v1, int v2) 
        : NimonspoliException("Error Dadu: Nilai (" + std::to_string(v1) + ", " + 
                              std::to_string(v2) + ") tidak valid! Harus 1-6.") {}
};

#endif