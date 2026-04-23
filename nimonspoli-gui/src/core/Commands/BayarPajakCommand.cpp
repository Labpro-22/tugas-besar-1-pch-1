#include "BayarPajakCommand.hpp"
#include "../Board/Board.hpp"

BayarPajakCommand::BayarPajakCommand(Player *p, TaxTile *tile, Bank *bank,
                                     TransactionLogger *logger, TaxConfig taxConfig, int turn)
    : p(p), tile(tile), bank(bank), logger(logger), taxConfig(taxConfig), turnNumber(turn) {}

void BayarPajakCommand::execute(GameMaster&)
{
    if (tile->getCode() == "PPH")
    {
        handlePPH();
    }
    else if (tile->getCode() == "PBM")
    {
        handlePBM();
    }
}

void BayarPajakCommand::handlePPH()
{
    // Dipanggil dari GUI setelah user memilih opsi (choice=1: flat, choice=2: persen)
    // Jangan dieksekusi langsung — lihat handlePPHChoice(int)
    // Method ini dibiarkan kosong; GUI akan memanggil handlePPHChoice secara langsung
}

void BayarPajakCommand::handlePPHChoice(int choice)
{
    int flatAmount = static_cast<int>(taxConfig.pphFlat);
    int pctAmount  = static_cast<int>(taxConfig.pphPercentage);

    if (choice == 1)
    {
        if (!p->canAfford(flatAmount))
            throw InsufficientFundsException(p->getUsername(), "Pajak Penghasilan (flat)");

        *p -= flatAmount;
        logger->addLog(turnNumber, p->getUsername(), "PAJAK",
                       "Bayar PPH flat M" + std::to_string(flatAmount));
    }
    else
    {
        int wealth = p->getWealth();
        int taxAmt = (wealth * pctAmount) / 100;

        if (!p->canAfford(taxAmt))
            throw InsufficientFundsException(p->getUsername(), "Pajak Penghasilan (%)");

        *p -= taxAmt;
        logger->addLog(turnNumber, p->getUsername(), "PAJAK",
                       "Bayar PPH " + std::to_string(pctAmount) +
                           "% = M" + std::to_string(taxAmt));
    }
}

void BayarPajakCommand::handlePBM()
{
    int flatAmount = static_cast<int>(taxConfig.pbmFlat);

    if (!p->canAfford(flatAmount))
        throw InsufficientFundsException(p->getUsername(), "Pajak Barang Mewah");

    *p -= flatAmount;
    logger->addLog(turnNumber, p->getUsername(), "PAJAK",
                   "Bayar PBM M" + std::to_string(flatAmount));
}