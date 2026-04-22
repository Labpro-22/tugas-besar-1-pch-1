#include "GameState.hpp"
#include "../Player/Player.hpp"
#include "../Board/Board.hpp"
#include "../Bank/Bank.hpp"
#include "../Dice/Dice.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../Card/CardDeck.hpp"
#include "../utils/TransactionLogger.hpp"

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

GameState::GameState()
    : currTurn(1), maxTurn(0), phase(GamePhase::NOT_STARTED),
      currPlayerIdx(0), hasExtraTurn(false), hasRolled(false), hasUsedCard(false),
      gameBoard(nullptr), gameBank(nullptr), gameDice(nullptr),
      auctionManager(nullptr), chanceCardDeck(nullptr),
      communityCardDeck(nullptr), skillCardDeck(nullptr), logger(nullptr) {}

GameState::GameState(
    int maxTurn,
    std::vector<Player*> players,
    Board*              board,
    Bank*               bank,
    Dice*               dice,
    AuctionManager*     auctionMgr,
    CardDeck<Card>*     chanceDeck,
    CardDeck<Card>*     communityDeck,
    CardDeck<Card>*     skillDeck,
    TransactionLogger*  log
)
    : currTurn(1), maxTurn(maxTurn), phase(GamePhase::NOT_STARTED),
      listPlayer(players), currPlayerIdx(0),
      hasExtraTurn(false), hasRolled(false), hasUsedCard(false),
      gameBoard(board), gameBank(bank), gameDice(dice),
      auctionManager(auctionMgr),
      chanceCardDeck(chanceDeck), communityCardDeck(communityDeck), skillCardDeck(skillDeck),
      logger(log) {}

// ─────────────────────────────────────────────
//  Getter: turn & fase
// ─────────────────────────────────────────────

int       GameState::getCurrTurn()  const { return currTurn; }
int       GameState::getMaxTurn()   const { return maxTurn; }
GamePhase GameState::getPhase()     const { return phase; }
bool      GameState::isGameOver()   const { return phase == GamePhase::GAME_OVER; }
bool      GameState::isMaxTurnReached() const {
    // maxTurn < 1 = mode BANKRUPTCY (tidak ada batas giliran)
    return maxTurn >= 1 && currTurn > maxTurn;
}

// ─────────────────────────────────────────────
//  Getter: pemain
// ─────────────────────────────────────────────

Player* GameState::getCurrPlayer() const {
    if (listPlayer.empty()) return nullptr;
    return listPlayer[currPlayerIdx];
}

int GameState::getCurrPlayerIdx() const { return currPlayerIdx; }

std::vector<Player*> GameState::getPlayers() const { return listPlayer; }

std::vector<Player*> GameState::getActivePlayers() const {
    std::vector<Player*> active;
    for (Player* p : listPlayer) {
        if (p && p->getStatus() != "BANKRUPT") {
            active.push_back(p);
        }
    }
    return active;
}

Player* GameState::getPlayerById(const std::string& id) const {
    for (Player* p : listPlayer) {
        if (p && p->getID() == id) return p;
    }
    return nullptr;
}

int  GameState::getPlayerCount()    const { return static_cast<int>(listPlayer.size()); }
bool GameState::getHasExtraTurn()   const { return hasExtraTurn; }
bool GameState::getHasRolled()      const { return hasRolled; }
bool GameState::getHasUsedCard()    const { return hasUsedCard; }

// ─────────────────────────────────────────────
//  Getter: entitas
// ─────────────────────────────────────────────

Board*             GameState::getBoard()          const { return gameBoard; }
Bank*              GameState::getBank()           const { return gameBank; }
Dice*              GameState::getDice()           const { return gameDice; }
AuctionManager*    GameState::getAuctionManager() const { return auctionManager; }
CardDeck<Card>*    GameState::getChanceDeck()     const { return chanceCardDeck; }
CardDeck<Card>*    GameState::getCommunityDeck()  const { return communityCardDeck; }
CardDeck<Card>*    GameState::getSkillDeck()      const { return skillCardDeck; }
TransactionLogger* GameState::getLogger()         const { return logger; }

// ─────────────────────────────────────────────
//  Setter
// ─────────────────────────────────────────────

void GameState::setPhase(GamePhase p)      { phase = p; }
void GameState::setHasExtraTurn(bool val)  { hasExtraTurn = val; }
void GameState::setHasRolled(bool val)     { hasRolled = val; }
void GameState::setHasUsedCard(bool val)   { hasUsedCard = val; }

// ─────────────────────────────────────────────
//  Navigasi giliran
// ─────────────────────────────────────────────

void GameState::advanceTurn() {
    currTurn++;
    hasExtraTurn = false;
    hasRolled    = false;
    hasUsedCard  = false;
}

void GameState::nextPlayer() {
    if (listPlayer.empty()) return;

    int total    = static_cast<int>(listPlayer.size());
    int attempts = 0;
    do {
        currPlayerIdx = (currPlayerIdx + 1) % total;
        attempts++;
    } while (listPlayer[currPlayerIdx]->getStatus() == "BANKRUPT"
             && attempts < total);

    hasExtraTurn = false;
    hasRolled    = false;
    hasUsedCard  = false;
}

void GameState::removePlayer(Player* p) {
    if (!p) return;
    p->setStatus("BANKRUPT");
}

int GameState::countActivePlayers() const {
    return static_cast<int>(getActivePlayers().size());
}