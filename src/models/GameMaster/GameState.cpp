#include "../../core/GameState/GameState.hpp"

// Constructor
GameState::GameState()
    : currTurn(0), maxTurn(0), activePlayerIdx(0) {}

// ── Turn ─────────────────────────────────────────────────────────────────────
int GameState::getCurrTurn() const {
    return currTurn;
}
void GameState::setCurrTurn(int turn) {
    currTurn = turn;
}

int GameState::getMaxTurn() const {
    return maxTurn;
}
void GameState::setMaxTurn(int max) {
    maxTurn = max;
}

// ── Players ───────────────────────────────────────────────────────────────────
Player* GameState::getPlayer() const {
    if (players.empty()) return nullptr;
    return players.front().get();
}

Player* GameState::getPlayerById(string id) const {
    for (auto& p : players) {
        if (p->getId() == id) return p.get();
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Player>>& GameState::getPlayers() const {
    return players;
}

// ── Active player ─────────────────────────────────────────────────────────────
int GameState::getActivePlayerIdx() const {
    return activePlayerIdx;
}
void GameState::setActivePlayerIdx(int idx) {
    activePlayerIdx = idx;
}

// ── Turn order ────────────────────────────────────────────────────────────────
const std::vector<std::shared_ptr<Player>>& GameState::getTurnOrder() const {
    return turnOrder;
}
void GameState::setTurnOrder(const std::vector<std::shared_ptr<Player>>& order) {
    turnOrder = order;
}

// ── Board ─────────────────────────────────────────────────────────────────────
Board* GameState::getGameBoard() const {
    return board.get();
}

// ── Card decks ────────────────────────────────────────────────────────────────
CardDeck* GameState::getChanceDeck() const {
    return chanceDeck.get();
}
CardDeck* GameState::getCommunityChestDeck() const {
    return communityChestDeck.get();
}