#include "BayarPajakCommand.hpp"
#include "../Board/Board.hpp"

BayarPajakCommand::BayarPajakCommand(Player *p, TaxTile *tile, Bank *bank,
                                     TransactionLogger *logger, TaxConfig taxConfig, int turn)
    : p(p), tile(tile), bank(bank), logger(logger), taxConfig(taxConfig), turnNumber(turn) {}

void BayarPajakCommand::execute(GameMaster& gameMaster)
{
    gm = &gameMaster;
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
        {
            // Tidak mampu bayar → trigger bankruptcy flow
            if (gm) {
                int liq = gm->handleDebtPayment(p, flatAmount, nullptr);
                if (liq == 2) {
                    gm->handleBankruptcy(p, gm->getState().getBank());
                }
            }
            return;
        }
        *p -= flatAmount;
        logger->addLog(turnNumber, p->getUsername(), "PAJAK",
                       "Bayar PPH flat M" + std::to_string(flatAmount));
    }
    else
    {
        int wealth = p->getWealth();
        int taxAmt = (wealth * pctAmount) / 100;

        if (!p->canAfford(taxAmt))
        {
            // Tidak mampu bayar → trigger bankruptcy flow
            if (gm) {
                int liq = gm->handleDebtPayment(p, taxAmt, nullptr);
                if (liq == 2) {
                    gm->handleBankruptcy(p, gm->getState().getBank());
                }
            }
            return;
        }
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
    {
        // Tidak mampu bayar → trigger bankruptcy flow
        if (gm) {
            int liq = gm->handleDebtPayment(p, flatAmount, nullptr);
            if (liq == 2) {
                gm->handleBankruptcy(p, gm->getState().getBank());
            }
        }
        return;
    }

    *p -= flatAmount;
    logger->addLog(turnNumber, p->getUsername(), "PAJAK",
                   "Bayar PBM M" + std::to_string(flatAmount));
}