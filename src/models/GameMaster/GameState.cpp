#include "include\models\GameState.hpp"

GameState(int currTurn, int maxTurn, int activePlayerIdx, std::vector<std::shared_ptr<Player>> turnOrder, std::vector<std::shared_ptr<Player>> players, std::unique_ptr<Board> board, std::unique_ptr<CardDeck> chanceDeck, std::unique_ptr<CardDeck> communityChestDeck)
        : {}

int GameState::getCurrTurn() const
{
    return this.currTurn;
}

void GameState::setCurrTurn(int turn){
    this.currTurn = turn;
}

int GameState::getMaxTurn() const
{
    return this.maxTurn;
}
void GameState::setMaxTurn(int max)
{
    this.maxTurn = max;
}

Player* GameState::getPlayer() const
{
    return this.players.front;
}

Player* GameState::getPlaterByid(int id) const
{
    auto it = this.players.begin()
    while(it != this.players.end()){
        // nnti yh kena squiggle hehe
    }
}

const std::vector<std::shared_ptr<Player>>& getPlayers() const
{
    return this.players;
}

Board* GameState::getGameBoard() const
{
    return this.board;
}

CardDeck* GameState::getChanceDeck() const
{

}
CardDeck* GameState::getCommunityChestDeck() const
{

}

int GameState::getActivePlayerIdx() const
{
    return this.activePlayerIdx;
}
void GameState::setActivePlayerIdx(int idx)
{
    this.activePlayerIdx = idx;
}

const GameState::std::vector<std::shared_ptr<Player>>& getTurnOrder() const { return turnOrder; }
int GameState::getActivePlayerIdx() const { return activePlayerIdx; }

void GameState::setTurnOrder(const std::vector<std::shared_ptr<Player>>& order) { turnOrder = order; }
void GameState::setActivePlayerIdx(int idx) { activePlayerIdx = idx; }


