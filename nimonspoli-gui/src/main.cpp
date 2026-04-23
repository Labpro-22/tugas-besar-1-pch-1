#include "../lib/raylib/include/raylib.h"
#include "views/GUIManager.hpp"
#include "views/screens/GameScreen.hpp"
#include "views/screens/MainMenuScreen.hpp"
#include "views/screens/WinScreen.hpp"

#include "core/GameMaster/GameMaster.hpp"
#include "core/GameState/GameState.hpp"
#include "core/Board/Board.hpp"
#include "core/Player/Player.hpp"
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
#include <memory>
#include <vector>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  main()
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    std::string title = "Nimonspoli";
    GUIManager gui(1920, 1080, title, 60);

    MainMenuScreen* menuScreen = new MainMenuScreen();
    GameScreen*     gameScreen = new GameScreen();
    WinScreen*      winScreen  = new WinScreen();

    gui.setScreen(menuScreen);

    // ── Objek game — di-reset setiap New Game ────────────────────────────
    GameMaster*     gameMaster    = nullptr;
    Bank*           bank          = nullptr;
    Dice*           dice          = nullptr;
    AuctionManager* auctionMgr    = nullptr;
    CardDeck<Card>* chanceDeck    = nullptr;
    CardDeck<Card>* communityDeck = nullptr;
    CardDeck<Card>* skillDeck     = nullptr;
    TransactionLogger* logger     = nullptr;
    Board*          board         = nullptr;
    std::vector<Player*> players;
    std::vector<std::unique_ptr<Property>> properties; // ownership semua Property*

    // Helper cleanup
    auto cleanupGame = [&]() {
        delete gameMaster;   gameMaster    = nullptr;
        delete bank;         bank          = nullptr;
        delete dice;         dice          = nullptr;
        delete auctionMgr;   auctionMgr    = nullptr;
        delete chanceDeck;   chanceDeck    = nullptr;
        delete communityDeck;communityDeck = nullptr;
        delete skillDeck;    skillDeck     = nullptr;
        delete logger;       logger        = nullptr;
        delete board;        board         = nullptr;
        for (auto* p : players) delete p;
        players.clear();
        properties.clear(); // unique_ptr auto-delete
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

    // ── Transisi: Menu → Game ─────────────────────────────────────────
    if (menuScreen->isReadyToStart()) {
        auto setup = menuScreen->getSetup();
        menuScreen->resetReady();
        cleanupGame();

        // Load config
        ConfigLoader cfg("config");
        auto propData      = cfg.loadProperties();
        auto railroadRent  = cfg.loadRailroad();
        auto utilityFactor = cfg.loadUtility();
        auto specialCfg    = cfg.loadSpecial();
        auto miscCfg       = cfg.loadMisc();
        auto actData = cfg.loadActions();

        properties = PropertyFactory::createProperties(
            propData, railroadRent, utilityFactor);

        bank          = new Bank(32, 12);
        dice          = new Dice();
        auctionMgr    = new AuctionManager();
        logger        = new TransactionLogger();
        chanceDeck    = new CardDeck<Card>();
        communityDeck = new CardDeck<Card>();
        skillDeck     = new CardDeck<Card>();

        board = BoardFactory::createBoard(propData, actData, specialCfg, chanceDeck, communityDeck, properties);

        if (setup.isLoadGame) {
            // ── LOAD GAME ─────────────────────────────────────────────
            // Buat dummy players dulu — loadPlayers() akan restore state-nya
            // Jumlah pemain dibaca dari file saat load
            // Kita perlu baca dulu berapa pemain di file sebelum buat Player*

            // Baca jumlah pemain dari file save
            std::ifstream peek(setup.saveFile);
            std::string line;
            std::getline(peek, line); // skip baris 1 (turn maxTurn)
            std::getline(peek, line); // baris 2 = jumlah pemain
            int savedCount = std::stoi(line);
            peek.close();

            for (int i = 0; i < savedCount; ++i)
                players.push_back(new Player("_tmp_" + std::to_string(i), miscCfg.initialBalance));

            GameState gs(
                miscCfg.maxTurn, players, board, bank, dice,
                auctionMgr, chanceDeck, communityDeck, skillDeck, logger
            );
            gameMaster = new GameMaster(gs);

            // Restore state dari file
            try {
                SaveLoadManager slm;
                slm.load(setup.saveFile, gameMaster->getState());
                std::cout << "[MUAT] Berhasil dimuat dari: " << setup.saveFile << std::endl;
                gameMaster->beginTurn();
            } catch (const std::exception& e) {
                std::cerr << "[MUAT] Gagal: " << e.what() << std::endl;
            }

            int count = (int)players.size();
            gui.setGameMaster(gameMaster);
            gameScreen->setGUIManager(&gui);
            gameScreen->setPlayerCount(count);
            gui.setScreen(gameScreen);
            // Tidak perlu beginTurn() — turn sudah di-restore dari file

        } else {
            // ── NEW GAME ──────────────────────────────────────────────
            int count = std::max(2, std::min(4, setup.playerCount));
            for (int i = 0; i < count; ++i) {
                std::string name = (i < (int)setup.names.size() && !setup.names[i].empty())
                                ? setup.names[i]
                                : ("Pemain" + std::to_string(i + 1));

                if (setup.isBot[i]) {
                    ComputerPlayer* p = new ComputerPlayer(name, miscCfg.initialBalance, setup.botDifficulty);
                    p->setPosition(1);  // ← tambah ini
                    players.push_back(p);
                } else {
                    Player* p = new Player(name, miscCfg.initialBalance);
                    p->setPosition(1);
                    players.push_back(p);
                }
            }

            GameState gs(
                miscCfg.maxTurn, players, board, bank, dice,
                auctionMgr, chanceDeck, communityDeck, skillDeck, logger
            );
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

        // ── Game logic: flush command → sync dice ─────────────────────────
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
                gameMaster->endTurn();
                gameMaster->beginTurn();
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

    // ── Cleanup final ─────────────────────────────────────────────────────
    cleanupGame();
    gui.setScreen(nullptr);
    delete menuScreen;
    delete gameScreen;
    delete winScreen;
    return 0;
}