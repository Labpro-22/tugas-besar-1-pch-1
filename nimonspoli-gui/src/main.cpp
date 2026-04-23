#include "../lib/raylib/include/raylib.h"
#include "views/GUIManager.hpp"
#include "views/screens/GameScreen.hpp"
#include "views/screens/MainMenuScreen.hpp"
#include "views/screens/WinScreen.hpp"

#include "core/GameMaster/GameMaster.hpp"
#include "core/GameState/GameState.hpp"
#include "core/Board/Board.hpp"
#include "core/Player/Player.hpp"
#include "core/ComputerPlayer/ComputerPlayer.hpp"  // ← TAMBAHAN
#include "core/Bank/Bank.hpp"
#include "core/Dice/Dice.hpp"
#include "core/AuctionManager/AuctionManager.hpp"
#include "core/Card/CardDeck.hpp"
#include "core/Card/Card.hpp"
#include "core/utils/TransactionLogger.hpp"
#include "core/utils/ConfigLoader.hpp"
#include "core/Property/PropertyFactory.hpp"
#include "core/Board/BoardFactory.hpp"
#include "core/utils/SaveLoadManager.hpp"
#include <fstream>
#include <sstream>   // ← TAMBAHAN untuk std::istringstream
#include <memory>
#include <vector>
#include <string>

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    std::string title = "Nimonspoli";
    GUIManager gui(1920, 1080, title, 60);

    MainMenuScreen* menuScreen = new MainMenuScreen();
    GameScreen*     gameScreen = new GameScreen();
    WinScreen*      winScreen  = new WinScreen();

    gui.setScreen(menuScreen);

    GameMaster*        gameMaster    = nullptr;
    Bank*              bank          = nullptr;
    Dice*              dice          = nullptr;
    AuctionManager*    auctionMgr    = nullptr;
    CardDeck<Card>*    chanceDeck    = nullptr;
    CardDeck<Card>*    communityDeck = nullptr;
    CardDeck<Card>*    skillDeck     = nullptr;
    TransactionLogger* logger        = nullptr;
    Board*             board         = nullptr;
    std::vector<Player*> players;
    std::vector<std::unique_ptr<Property>> properties;

    auto cleanupGame = [&]() {
        delete gameMaster;    gameMaster    = nullptr;
        delete bank;          bank          = nullptr;
        delete dice;          dice          = nullptr;
        delete auctionMgr;    auctionMgr    = nullptr;
        delete chanceDeck;    chanceDeck    = nullptr;
        delete communityDeck; communityDeck = nullptr;
        delete skillDeck;     skillDeck     = nullptr;
        delete logger;        logger        = nullptr;
        delete board;         board         = nullptr;
        for (auto* p : players) delete p;
        players.clear();
        properties.clear();
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // ── Transisi: Menu → Game ─────────────────────────────────────────
        if (menuScreen->isReadyToStart()) {
            auto setup = menuScreen->getSetup();
            menuScreen->resetReady();
            cleanupGame();

            ConfigLoader cfg("config");
            auto propData      = cfg.loadProperties();
            auto railroadRent  = cfg.loadRailroad();
            auto utilityFactor = cfg.loadUtility();
            auto specialCfg    = cfg.loadSpecial();
            auto miscCfg       = cfg.loadMisc();
            auto actData       = cfg.loadActions();

            properties = PropertyFactory::createProperties(propData, railroadRent, utilityFactor);

            bank          = new Bank(32, 12);
            dice          = new Dice();
            auctionMgr    = new AuctionManager();
            logger        = new TransactionLogger();
            chanceDeck    = new CardDeck<Card>();
            communityDeck = new CardDeck<Card>();
            skillDeck     = new CardDeck<Card>();

            board = BoardFactory::createBoard(propData, actData, specialCfg,
                                              chanceDeck, communityDeck, properties);

            if (setup.isLoadGame) {
                // ── LOAD GAME ─────────────────────────────────────────────
                // Peek file dulu untuk tahu tipe tiap player (HUMAN/COM)
                // sebelum construct — karena tidak bisa downcast setelah buat

                struct PlayerInfo {
                    std::string   name;
                    bool          isBot;
                    COMDifficulty diff;
                };
                std::vector<PlayerInfo> infos;

                {
                    std::ifstream peek(setup.saveFile);
                    std::string line;
                    std::getline(peek, line); // baris 1: turn maxTurn
                    std::getline(peek, line); // baris 2: jumlah pemain
                    int savedCount = std::stoi(line);

                    for (int i = 0; i < savedCount; i++) {
                        std::getline(peek, line); // baris player

                        std::vector<std::string> tok;
                        std::istringstream ss(line);
                        std::string w;
                        while (ss >> w) tok.push_back(w);

                        PlayerInfo info;
                        info.name  = tok.size() > 0 ? tok[0] : "_tmp_";
                        info.isBot = tok.size() > 4 && tok[4] == "COM";
                        info.diff  = COMDifficulty::MEDIUM;
                        if (tok.size() > 5) {
                            if (tok[5] == "EASY") info.diff = COMDifficulty::EASY;
                            if (tok[5] == "HARD") info.diff = COMDifficulty::HARD;
                        }
                        infos.push_back(info);

                        std::getline(peek, line); // jumlah kartu
                        int cardCount = line.empty() ? 0 : std::stoi(line);
                        for (int c = 0; c < cardCount; c++)
                            std::getline(peek, line); // skip baris kartu
                    }
                }

                // Construct player dengan tipe yang benar
                for (auto& info : infos) {
                    Player* p;
                    if (info.isBot)
                        p = new ComputerPlayer(info.name, miscCfg.initialBalance, info.diff);
                    else
                        p = new Player(info.name, miscCfg.initialBalance);
                    p->setPosition(1);
                    players.push_back(p);
                }

                GameState gs(miscCfg.maxTurn, players, board, bank, dice,
                             auctionMgr, chanceDeck, communityDeck, skillDeck, logger);
                gameMaster = new GameMaster(gs);

                try {
                    SaveLoadManager slm;
                    slm.load(setup.saveFile, gameMaster->getState());
                    gameMaster->beginTurn();
                    std::cout << "[MUAT] Berhasil dimuat dari: " << setup.saveFile << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[MUAT] Gagal: " << e.what() << std::endl;
                }

                int count = (int)players.size();
                gui.setGameMaster(gameMaster);
                gameScreen->setGUIManager(&gui);
                gameScreen->setPlayerCount(count);
                gui.setScreen(gameScreen);

            } else {
                // ── NEW GAME ──────────────────────────────────────────────
                int count = std::max(2, std::min(4, setup.playerCount));
                for (int i = 0; i < count; ++i) {
                    std::string name = (i < (int)setup.names.size() && !setup.names[i].empty())
                                     ? setup.names[i]
                                     : ("Pemain" + std::to_string(i + 1));

                    Player* p;
                    if (setup.isBot[i])
                        p = new ComputerPlayer(name, miscCfg.initialBalance, setup.botDifficulty);
                    else
                        p = new Player(name, miscCfg.initialBalance);
                    p->setPosition(1);
                    players.push_back(p);
                }

                GameState gs(miscCfg.maxTurn, players, board, bank, dice,
                             auctionMgr, chanceDeck, communityDeck, skillDeck, logger);
                gameMaster = new GameMaster(gs);

                gui.setGameMaster(gameMaster);
                gameScreen->setGUIManager(&gui);
                gameScreen->setPlayerCount(count);
                gui.setScreen(gameScreen);

                gameMaster->beginTurn();
            }
        }

        // ── Transisi: Game → Win ──────────────────────────────────────────
        if (gameScreen->isGameOver()) {
            std::vector<PlayerResult> results;
            for (int i = 0; i < (int)players.size(); ++i) {
                PlayerResult r;
                r.username = players[i]->getUsername();
                r.money    = players[i]->getBalance();
                r.color    = gameScreen->playerColors[i];
                results.push_back(r);
            }
            winScreen->setResults(results, WinScenario::MAX_TURN);
            gameScreen->gameOver = false;
            gui.setGameMaster(nullptr);
            gui.setScreen(winScreen);
        }

        // ── Transisi: Win → Menu ──────────────────────────────────────────
        if (winScreen->goToMainMenu()) {
            winScreen->reset();
            cleanupGame();
            gui.setScreen(menuScreen);
        }
        if (winScreen->goToExit()) break;

        // ── Game logic ────────────────────────────────────────────────────
        gui.flushCommands();

        if (gui.getCurrentScreen()) {
            auto* gs = dynamic_cast<GameScreen*>(gui.getCurrentScreen());
            if (gs) gs->syncDiceResult();
        }

        // ── COM auto-play ─────────────────────────────────────────────────
        if (gameMaster) {
            GameState& state = gameMaster->getState();
            Player* curr = state.getCurrPlayer();
            ComputerPlayer* com = dynamic_cast<ComputerPlayer*>(curr);
            if (com && state.getPhase() == GamePhase::PLAYER_TURN
                    && !state.getHasRolled()) {
                com->executeTurn(*gameMaster);
                if (state.getPhase() != GamePhase::GAME_OVER) {
                    gameMaster->endTurn();
                    gameMaster->beginTurn();
                }
            }
        }

        if (gui.getCurrentScreen()) {
            auto* gs = dynamic_cast<GameScreen*>(gui.getCurrentScreen());
            if (gs) gs->syncDiceResult();
        }

        // ── Update + Render ───────────────────────────────────────────────
        BeginDrawing();
        if (gui.getCurrentScreen()) {
            gui.getCurrentScreen()->update(dt);
            gui.getCurrentScreen()->render(gui.getWindow());
        }
        EndDrawing();
    }

    cleanupGame();
    gui.setScreen(nullptr);
    delete menuScreen;
    delete gameScreen;
    delete winScreen;
    return 0;
}