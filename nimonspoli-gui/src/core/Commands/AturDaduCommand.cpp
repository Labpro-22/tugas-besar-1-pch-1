#include "AturDaduCommand.hpp"
#include "../Exceptions/DiceExceptions.hpp"
#include "../Dice/Dice.hpp"
#include <iostream>


// --- Implementasi AturDaduCommand ---
AturDaduCommand::AturDaduCommand(Dice& d, int v1, int v2) 
    : dice(d), targetVal1(v1), targetVal2(v2) {}

void AturDaduCommand::execute(GameMaster& gm) {
    // 1. Validasi rentang nilai dadu
    if (targetVal1 < 1 || targetVal1 > 6 || targetVal2 < 1 || targetVal2 > 6) {
        // Melempar exception jika nilai di luar batas 1-6
        throw InvalidDiceValueException(targetVal1, targetVal2);
    }

    // 2. Jika valid, atur dadu secara manual
    dice.setManual(targetVal1, targetVal2);

    // 3. Konfirmasi output ke terminal
    std::cout << "Dadu berhasil diatur secara manual!" << std::endl;
    std::cout << "Dadu 1 = " << dice.getDaduVal1() << ", Dadu 2 = " << dice.getDaduVal2() 
              << " (Total: " << dice.getTotal() << ")" << std::endl;
    
    // Catatan: Jika ini murni untuk pengaturan state sebelum LemparDaduCommand, 
    // Anda tidak perlu menggerakkan player di sini. 
    // Pergerakan player tetap dieksekusi saat command melempar dadu atau command jalan dipanggil.
}