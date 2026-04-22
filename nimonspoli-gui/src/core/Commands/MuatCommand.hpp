#ifndef MUAT_COMMAND_HPP
#define MUAT_COMMAND_HPP

#include "Command.hpp"
#include <string>
// Forward declarations
class SaveLoadManager;
class InputHandler;
class DisplayManager;
class GameMaster;

// ─────────────────────────────────────────────
//  MuatCommand
//  Bertanggung jawab menjalankan perintah MUAT
//
//  execute() flow:
//  1. Tanya nama file ke user via InputHandler
//  2. Cek apakah file ada
//  3. Kalau tidak ada → tampilkan error
//  4. Kalau ada → load, restore ke GameState
//  5. Tampilkan hasil via DisplayManager
//
//  Catatan spec: MUAT hanya bisa dilakukan
//  saat program baru dijalankan (sebelum game
//  dimulai) — validasi ini ada di GameMaster
// ─────────────────────────────────────────────
class MuatCommand : public Command {
private:
    SaveLoadManager& saveLoad;
    InputHandler&    input;
    DisplayManager&  display;

public:
    MuatCommand(SaveLoadManager& saveLoad,
                InputHandler&    input,
                DisplayManager&  display);

    void execute(GameMaster& gm) override;
};

#endif