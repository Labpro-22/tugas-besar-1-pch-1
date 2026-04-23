#ifndef BAYARPAJAKCOMMAND_HPP
#define BAYARPAJAKCOMMAND_HPP

#include "Command.hpp"
#include "../utils/ConfigLoader.hpp"
#include <string>

class Player;

class BayarPajakCommand : public Command {
private:
    Player*     currentPlayer;
    TaxConfig   taxConfig;
    std::string taxType;  // "PPH" atau "PBM"
    int         option;
    /*
     * Option (hanya relevan untuk PPH):
     *   1 = Bayar flat (pphFlat dari config)
     *   2 = Bayar persentase kekayaan (pphPercentage dari config)
     * Untuk PBM, option diabaikan (selalu pbmFlat dari config).
     *
     * Catatan: option ditentukan oleh GUI sebelum Command di-instantiate,
     * sesuai dengan alur GUI yang memilih lewat dialog/button.
     */

public:
    BayarPajakCommand(Player* p, TaxConfig taxConfig, std::string taxType, int option);

    void execute(GameMaster& gm) override;
};

#endif