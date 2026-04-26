#ifndef SAVE_LOAD_MANAGER_HPP
#define SAVE_LOAD_MANAGER_HPP
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

class GameState;
class Player;
class Board;
class TransactionLogger;

using namespace std;

//  Format file 
//  <TURN> <MAX_TURN>
//  <JUMLAH_PEMAIN>
//  <STATE_PEMAIN_1..N>
//  <URUTAN_GILIRAN>
//  <GILIRAN_AKTIF>
//  <STATE_PROPERTI>
//  <STATE_DECK>
//  <STATE_LOG>

class SaveLoadManager{
    public:
        SaveLoadManager();

        void save(const GameState& state, const string& filename);
        void load(const string& filename, GameState& state);

        bool fileExists(const string& filename) const;
        

        // Save Helpers
        void savePlayers(ofstream& out, const GameState& state);
        void saveProperties(ofstream& out, const GameState& state);
        void saveDeck(ofstream& out, const GameState& state);
        void saveLogs(ofstream& out, const GameState& state);

        // Load Helpers
        void loadPlayers(ifstream& in, GameState& state);
        void loadProperties(ifstream& in, GameState& state);
        void loadDeck(ifstream& in, GameState& state);
        void loadLogs(ifstream& in, GameState& state);

        vector<string> tokenize(const string& line);
};

#endif