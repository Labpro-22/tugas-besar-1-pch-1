#ifndef LEMPARDADUCOMMAND_HPP
#define LEMPARDADUCOMMAND_HPP

#include "Command.hpp" // Asumsi ada interface ICommand atau Command dasar

// Forward declarations untuk mengurangi ketergantungan header
class GameMaster;
class Player;
class Dice;

class LemparDaduCommand : public Command {
private:
    GameMaster& gameMaster; // Menyimpan referensi sesuai dengan kaidah desain di Laporan M1 Anda
    Player* currentPlayer;
    Dice& dice;

public:
    // Konstruktor menerima referensi/pointer ke entitas yang dibutuhkan
    LemparDaduCommand(GameMaster& gm, Player* player, Dice& d);

    // Override metode execute dari kelas induk Command
    void execute(GameMaster& gm) override;
};

#endif