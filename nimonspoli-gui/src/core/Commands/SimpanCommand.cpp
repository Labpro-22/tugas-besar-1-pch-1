#include "SimpanCommand.hpp"
#include "../utils/SaveLoadManager.hpp"
#include "../utils/InputHandler.hpp"
#include "../utils/DisplayManager.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Exceptions/SaveLoadException.hpp"
#include "../Player/Player.hpp"
SimpanCommand::SimpanCommand(SaveLoadManager& saveLoad,
                             InputHandler&    input,
                             DisplayManager&  display)
    : saveLoad(saveLoad), input(input), display(display) {}

// ─────────────────────────────────────────────
//  execute
//  Sesuai spec: tanya nama file, cek timpa,
//  simpan, tampilkan hasil
// ─────────────────────────────────────────────
void SimpanCommand::execute(GameMaster& gm) {
    // 1. Cek apakah fase valid untuk simpan
    //    Spec: hanya bisa di awal giliran sebelum lempar dadu
    auto& state = gm.getState();
    if (state.getHasRolled()) {
        display.print("Tidak bisa menyimpan setelah melempar dadu.");
        display.print("Simpan hanya bisa dilakukan di awal giliran.");
        return;
    }

    // 2. Tanya nama file
    std::string filename = input.readString("Masukkan nama file save (contoh: save.txt): ");

    // Tambahkan folder data/ kalau user tidak tulis path
    if (filename.find('/') == std::string::npos &&
        filename.find('\\') == std::string::npos)
        filename = "data/" + filename;

    // 3. Cek apakah file sudah ada → tanya konfirmasi timpa
    if (saveLoad.fileExists(filename)) {
        display.print("File '" + filename + "' sudah ada.");
        bool confirm = input.readConfirm("Timpa file ini? (y/n): ");
        if (!confirm) {
            display.print("Penyimpanan dibatalkan.");
            return;
        }
    }

    // 4. Simpan
    try {
        saveLoad.save(state, filename);

        // Log ke TransactionLogger
        gm.log(state.getCurrPlayer()->getUsername(),
               "SIMPAN",
               "Game disimpan ke " + filename);

        display.print("Game berhasil disimpan ke '" + filename + "'.");

    } catch (const SaveFileNotFoundException& e) {
        display.print("Gagal menyimpan: " + std::string(e.what()));
    } catch (const std::exception& e) {
        display.print("Gagal menyimpan: " + std::string(e.what()));
    }
}