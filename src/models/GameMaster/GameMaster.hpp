#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include <vector>
#include <string>
#include <memory>
#include "include\models\GameState.hpp" 
#include "include\views\TransactionLogger.hpp"

class GameState {
private:
    GameState gameState; // erm ga paham
    TransactionLogger logger;

public:
    GameMaster(GameState gameState);
    void start();
    void handleTurn(std::string cmd);
    void nextTurn();
    bool isCompleted() const;
    GameState getGameState() const;
    void handle(GameState state);
    void handleBankruptcy(Player* from, Player* to);
    void handleBankruptcy(Player* from, Bank* to);
};

#endif