#ifndef SIMPAN_COMMAND_HPP
#define SIMPAN_COMMAND_HPP

#include "Command.hpp"

// Forward declarations
class SaveLoadManager;
class InputHandler;
class DisplayManager;
class GameMaster;

// ─────────────────────────────────────────────
//  SimpanCommand
//  Bertanggung jawab menjalankan perintah SIMPAN
//
//  execute() flow:
//  1. Tanya nama file ke user via InputHandler
//  2. Cek apakah file sudah ada
//  3. Kalau sudah ada → tanya konfirmasi timpa
//  4. Kalau tidak ada atau dikonfirmasi → simpan
//  5. Tampilkan hasil via DisplayManager
// ─────────────────────────────────────────────
class SimpanCommand : public Command {
private:
    SaveLoadManager& saveLoad;
    InputHandler&    input;
    DisplayManager&  display;

public:
    SimpanCommand(SaveLoadManager& saveLoad,
                  InputHandler&    input,
                  DisplayManager&  display);

    void execute(GameMaster& gm) override;
};

#endif