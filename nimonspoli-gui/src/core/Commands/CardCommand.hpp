#pragma once
#include "./Command.hpp"
#include "../Player/Player.hpp"
#include "../Card/Card.hpp"
#include "../Card/CardDeck.hpp"
#include "../GameState/GameState.hpp"
#include "../utils/TransactionLogger.hpp"
#include "../Exceptions/FinancialException.hpp"
#include "../Board/Board.hpp"
#include <string>

class CardCommand : public Command
{
private:
    Player *p;
    CardDeck<Card> *deck;
    GameState *gs;
    TransactionLogger *logger;
    int turnNumber;
    std::string deckLabel;
    std::string lastDescription; // deskripsi kartu terakhir yang ditarik

public:
    CardCommand(Player *p, CardDeck<Card> *deck, GameState *gs,
                TransactionLogger *logger, int turnNumber,
                const std::string &deckLabel);
    void execute(GameMaster& ) override;

    // GUI API: ambil deskripsi kartu terakhir untuk ditampilkan di CardDialog
    const std::string& getLastDescription() const { return lastDescription; }
    const std::string& getDeckLabel()       const { return deckLabel; }
};