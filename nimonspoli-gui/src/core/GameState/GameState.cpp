#include "GameState.hpp"
#include "../Player/Player.hpp"
#include "../Board/Board.hpp"
#include "../Bank/Bank.hpp"
#include "../Dice/Dice.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../utils/TransactionLogger.hpp"
#include "../Card/CardDeck.hpp"
#include "../Card/SkillCard.hpp"

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

GameState::GameState()
    : currTurn(1), maxTurn(0), phase(GamePhase::NOT_STARTED),
      currPlayerIdx(0), hasExtraTurn(false), hasRolled(false), hasUsedCard(false),
      gameBoard(nullptr), gameBank(nullptr), gameDice(nullptr),
      auctionManager(nullptr), chanceCardDeck(nullptr),
      communityCardDeck(nullptr), skillCardDeck(nullptr), logger(nullptr), gameMaster(nullptr) {}

GameState::GameState(
    int maxTurn,
    std::vector<Player *> players,
    Board *board,
    Bank *bank,
    Dice *dice,
    AuctionManager *auctionMgr,
    CardDeck<Card> *chanceDeck,
    CardDeck<Card> *communityDeck,
    CardDeck<SkillCard> *skillDeck,
    TransactionLogger *log, TaxConfig taxcfg)
    : currTurn(1), maxTurn(maxTurn), phase(GamePhase::NOT_STARTED),
      listPlayer(players), currPlayerIdx(0),
      hasExtraTurn(false), hasRolled(false), hasUsedCard(false),
      gameBoard(board), gameBank(bank), gameDice(dice),
      auctionManager(auctionMgr),
      chanceCardDeck(chanceDeck),
      communityCardDeck(communityDeck),
      skillCardDeck(skillDeck),
      logger(log),
      gameMaster(nullptr),
      taxcfg(taxcfg) {}

// ─────────────────────────────────────────────
//  Getter: turn & fase
// ─────────────────────────────────────────────

int GameState::getCurrTurn() const { return currTurn; }
int GameState::getMaxTurn() const { return maxTurn; }
GamePhase GameState::getPhase() const { return phase; }
bool GameState::isGameOver() const { return phase == GamePhase::GAME_OVER; }
bool GameState::isMaxTurnReached() const
{
    // maxTurn < 1 = mode BANKRUPTCY (tidak ada batas giliran)
    return maxTurn >= 1 && currTurn > maxTurn;
}

// ─────────────────────────────────────────────
//  Getter: pemain
// ─────────────────────────────────────────────

Player *GameState::getCurrPlayer() const
{
    if (listPlayer.empty())
        return nullptr;
    return listPlayer[currPlayerIdx];
}

int GameState::getCurrPlayerIdx() const { return currPlayerIdx; }

std::vector<Player *> GameState::getPlayers() const { return listPlayer; }

std::vector<Player *> GameState::getActivePlayers() const
{
    std::vector<Player *> active;
    for (Player *p : listPlayer)
    {
        if (p && p->getStatus() != PlayerStatus::BANKRUPT)
        {
            active.push_back(p);
        }
    }
    return active;
}

Player *GameState::getPlayerById(const std::string &id) const
{
    for (Player *p : listPlayer)
    {
        if (p && p->getUsername() == id)
            return p;
    }
    return nullptr;
}

int GameState::getPlayerCount() const { return static_cast<int>(listPlayer.size()); }
bool GameState::getHasExtraTurn() const { return hasExtraTurn; }
bool GameState::getHasRolled() const { return hasRolled; }
bool GameState::getHasUsedCard() const { return hasUsedCard; }

// ─────────────────────────────────────────────
//  Getter: entitas
// ─────────────────────────────────────────────

Board *GameState::getBoard() const { return gameBoard; }
Bank *GameState::getBank() const { return gameBank; }
Dice *GameState::getDice() const { return gameDice; }
AuctionManager *GameState::getAuctionManager() const { return auctionManager; }
CardDeck<Card> *GameState::getChanceDeck() const { return chanceCardDeck; }
CardDeck<Card> *GameState::getCommunityDeck() const { return communityCardDeck; }
CardDeck<SkillCard> *GameState::getSkillDeck() const
{
    return skillCardDeck;
}
TransactionLogger *GameState::getLogger() const { return logger; }
GameMaster *GameState::getGameMaster() const { return gameMaster; }
TaxConfig GameState::getTaxConfig() const { return taxcfg; }

// ─────────────────────────────────────────────
//  Setter
// ─────────────────────────────────────────────

void GameState::setPhase(GamePhase p) { phase = p; }
void GameState::setHasExtraTurn(bool val) { hasExtraTurn = val; }
void GameState::setHasRolled(bool val) { hasRolled = val; }
void GameState::setHasUsedCard(bool val) { hasUsedCard = val; }
void GameState::setGameMaster(GameMaster *gm) { gameMaster = gm; }

// ─────────────────────────────────────────────
//  Navigasi giliran
// ─────────────────────────────────────────────

void GameState::advanceTurn()
{
    currTurn++;
    hasExtraTurn = false;
    hasRolled = false;
    hasUsedCard = false;
}

void GameState::nextPlayer()
{
    if (listPlayer.empty())
        return;

    int total = static_cast<int>(listPlayer.size());
    int attempts = 0;
    do
    {
        currPlayerIdx = (currPlayerIdx + 1) % total;
        attempts++;
        if (attempts >= total)
            return;
    } while (listPlayer[currPlayerIdx]->getStatus() == PlayerStatus::BANKRUPT && attempts < total);

    hasExtraTurn = false;
    hasRolled = false;
    hasUsedCard = false;
}

void GameState::removePlayer(Player *p)
{
    if (!p)
        return;
    p->setStatus(PlayerStatus::BANKRUPT);
}

int GameState::countActivePlayers() const
{
    return static_cast<int>(getActivePlayers().size());
}

void GameState::setCurrTurn(int t) { currTurn = t; }
void GameState::setCurrPlayerIdx(int i) { currPlayerIdx = i; }
void GameState::setMaxTurn(int m) { maxTurn = m; }

Player *GameState::getPendingSkillDropPlayer() const { return pendingSkillDropPlayer; }
const std::string &GameState::getPendingSkillDropMessage() const { return pendingSkillDropMessage; }

void GameState::setPendingSkillDrop(Player *p, const std::string &msg)
{
    pendingSkillDropPlayer = p;
    pendingSkillDropMessage = msg;
}

void GameState::clearPendingSkillDrop()
{
    pendingSkillDropPlayer = nullptr;
    pendingSkillDropMessage.clear();
}

Property *GameState::getPropertyByCode(const std::string &code) const
{
    Board *board = getBoard();
    if (!board)
        return nullptr;

    for (int i = 0; i < board->getSize(); ++i)
    {
        Tile *tile = board->getTile(i);
        if (!tile || tile->getCode() != code)
            continue;

        if (PropertyTile *pt = dynamic_cast<PropertyTile *>(tile))
            return pt->getProperty();

        if (RailRoadTile *rt = dynamic_cast<RailRoadTile *>(tile))
            return rt->getProperty();

        if (UtilityTile *ut = dynamic_cast<UtilityTile *>(tile))
            return ut->getProperty();

        return nullptr;
    }

    return nullptr;
}