#pragma once
#include "../Bank/Bank.hpp"
#include "../Board/Board.hpp"
#include "../utils/TransactionLogger.hpp"
#include "../Exceptions/FinancialException.hpp"
#include "./Command.hpp"
#include "../Player/Player.hpp"
#include "../utils/ConfigLoader.hpp"
class TaxTile;
class BayarPajakCommand : public Command
{
private:
    Player *p;
    TaxTile *tile;
    Bank *bank;
    TransactionLogger *logger;
    TaxConfig taxConfig;
    int turnNumber;

public:
    BayarPajakCommand(Player *p, TaxTile *tile, Bank *bank,
                      TransactionLogger *logger, TaxConfig taxConfig, int turn);
    void execute(GameMaster& gm) override;

public:
    // Dipanggil oleh TaxDialog setelah user memilih:
    //   choice = 1 → bayar flat PPH
    //   choice = 2 → bayar persentase PPH
    void handlePPHChoice(int choice);

    // Getter untuk data yang ditampilkan di dialog sebelum user memilih
    int getPphFlatAmount() const { return static_cast<int>(taxConfig.pphFlat); }
    int getPphPctAmount()  const { return static_cast<int>(taxConfig.pphPercentage); }
    Player* getPlayer()    const { return p; }

private:
    void handlePPH();
    void handlePBM();
};