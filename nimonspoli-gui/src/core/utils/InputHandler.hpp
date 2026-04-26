#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <string>

// ─────────────────────────────────────────────
//  InputHandler
//  Satu-satunya pintu masuk input dari user
//  via keyboard (cin). Tidak ada atribut —
//  semua method langsung baca dari cin.
//
//  Dipakai oleh: SimpanCommand, MuatCommand,
//                CetakAktaCommand
// ─────────────────────────────────────────────
class InputHandler {
public:
    // Baca satu baris perintah, ubah ke UPPERCASE
    // Contoh: "lempar dadu" → "LEMPAR DADU"
    std::string readCommand();

    // Tampilkan prompt, baca integer, ulangi sampai
    // nilai dalam range [min, max]
    int readInt(const std::string& prompt, int min, int max);

    // Tampilkan prompt, baca teks bebas (satu baris)
    std::string readString(const std::string& prompt);

    // Tampilkan prompt, baca y/n, return bool
    // 'y' atau 'Y' → true, selain itu → false
    bool readConfirm(const std::string& prompt);
};

#endif