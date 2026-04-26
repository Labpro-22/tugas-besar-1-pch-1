#include "MuatCommand.hpp"

#include <string>
#include <exception>

MuatCommand::MuatCommand(SaveLoadManager& saveLoad,
                         InputHandler&    input,
                         DisplayManager&  display)
    : saveLoad(saveLoad), input(input), display(display) {}

// ─────────────────────────────────────────────
//  execute
//  Sesuai spec: tanya nama file, cek ada,
//  load, restore ke GameState
// ─────────────────────────────────────────────
void MuatCommand::execute(GameMaster& gm) {
    // 1. Cek apakah fase valid untuk muat
    //    Spec: hanya bisa saat program baru dijalankan
    //    sebelum game dimulai (NOT_STARTED)
    auto& state = gm.getState();
    if (state.getPhase() != GamePhase::NOT_STARTED) {
        display.print("MUAT hanya bisa dilakukan sebelum permainan dimulai.");
        return;
    }

    // 2. Tanya nama file
    std::string filename = input.readString("Masukkan nama file save (contoh: save.txt): ");

    // Tambahkan folder data/ kalau user tidak tulis path
    if (filename.find('/') == std::string::npos &&
        filename.find('\\') == std::string::npos)
        filename = "data/" + filename;

    // 3. Cek apakah file ada
    if (!saveLoad.fileExists(filename)) {
        display.print("File '" + filename + "' tidak ditemukan.");
        return;
    }

        // 4. Load dan restore ke GameState
    try {
        saveLoad.load(filename, state);

        // Log ke TransactionLogger agar tersimpan di save berikutnya
        gm.log("SYSTEM", "MUAT", "Game_dimuat_dari_" + filename);

        display.print("Game berhasil dimuat dari '" + filename + "'.");
        display.print("Melanjutkan giliran: " +
                      state.getCurrPlayer()->getUsername());

    } catch (const SaveFileNotFoundException& e) {
        display.print("File tidak ditemukan: " + std::string(e.what()));
    } catch (const InvalidSaveFormatException& e) {
        display.print("Format file rusak: " + std::string(e.what()));
    } catch (const std::exception& e) {
        display.print("Gagal memuat: " + std::string(e.what()));
    }
}