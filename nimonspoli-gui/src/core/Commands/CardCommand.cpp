#include "CardCommand.hpp"

CardCommand::CardCommand(Player *p, CardDeck<Card> *deck, GameState *gs,
                         TransactionLogger *logger, int turnNumber,
                         const std::string &deckLabel)
    : p(p), deck(deck), gs(gs), logger(logger),
      turnNumber(turnNumber), deckLabel(deckLabel) {}

void CardCommand::execute(GameMaster& )
{
    if (!deck)
        return;

    Card *kartu = deck->draw();
    if (!kartu)
        return;

    lastDescription = kartu->getDescription();

    try
    {
        kartu->execute(*p, *gs);
    }
    catch (const InsufficientFundsException &e)
    {
        deck->discard(kartu);
        throw;
    }

    if (logger)
    {
        logger->addLog(turnNumber, p->getUsername(), "KARTU",
                       deckLabel + ": " + kartu->getDescription());
    }

    deck->discard(kartu);
}