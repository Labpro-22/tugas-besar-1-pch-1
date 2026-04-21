#include "GameMaster.hpp"

GameMaster::GameMaster(GameState gameState) 
    : gameState(gameState) {}

void GameMaster::start() 
{

}

void GameMaster::handleTurn(std::string cmd)
{

}

void GameMaster::nextTurn();
{

}

bool GameMaster::isCompleted() const;
{
    return false; // nanti fix based on wincon
}

GameState GameMaster::getGameState() const;
{
    return this.gameState;
}

void GameMaster::handle(GameState state);
{

}

void GameMaster::handleBankruptcy(Player* from, Player* to);
{

}

void GameMaster::handleBankruptcy(Player* from, Bank* to);
{

}