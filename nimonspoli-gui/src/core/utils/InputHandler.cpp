#include "InputHandler.hpp"
#include <iostream>
#include <algorithm>
#include <limits>

// ─── readCommand ─────────────────────────────────────────────────────────────
// Baca satu baris, ubah semua huruf ke uppercase
// Contoh: "lempar dadu" → "LEMPAR DADU"
std::string InputHandler::readCommand() {
    std::string input;
    std::getline(std::cin, input);

    // Ubah ke uppercase
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);

    return input;
}

// ─── readInt ─────────────────────────────────────────────────────────────────
// Ulangi prompt sampai user masukkan angka yang valid dalam range [min, max]
// Validasi range di sini — Command tidak perlu validasi lagi
int InputHandler::readInt(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (value >= min && value <= max)
                return value;
            std::cout << "Masukkan angka antara " << min << " dan " << max << ".\n";
        } else {
            // Input bukan angka — clear error state
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Input tidak valid. Masukkan angka.\n";
        }
    }
}

// ─── readString ──────────────────────────────────────────────────────────────
// Tampilkan prompt, baca satu baris teks bebas
std::string InputHandler::readString(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// ─── readConfirm ─────────────────────────────────────────────────────────────
// Tampilkan prompt, baca y/n
// 'y' atau 'Y' → true, selain itu → false
bool InputHandler::readConfirm(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return (!input.empty() && (input[0] == 'y' || input[0] == 'Y'));
}