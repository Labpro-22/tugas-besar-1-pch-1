#include "../lib/raylib/include/raylib.h"
#include "views/GUIManager.hpp"
#include "views/screens/GameScreen.hpp"
#include "views/screens/MainMenuScreen.hpp"
#include "views/screens/WinScreen.hpp"
#include "views/screens/GameResult.hpp"
#include "core/utils/ConfigLoader.hpp"
#include "core/utils/TransactionLogger.hpp"
#include "core/GameState/GameState.hpp"
#include "core/GameMaster/GameMaster.hpp"
#include "core/Player/Player.hpp"
#include "core/Board/Board.hpp"
#include "core/Bank/Bank.hpp"
#include "core/Dice/Dice.hpp"
#include "core/AuctionManager/AuctionManager.hpp"
#include "core/Card/CardDeck.hpp"
#include "core/Card/DeckFactory.hpp"
#include "core/Property/PropertyFactory.hpp"
#include <iostream>
#include <vector>

int main() {
    // InitWindow HARUS sebelum GUIManager
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Nimonspoli");
    SetTargetFPS(60);

    try {
        std::string title = "Nimonspoli";
        GUIManager gui(1920, 1080, title, 60);

        MainMenuScreen* menuScreen = new MainMenuScreen();
        GameScreen*     gameScreen = new GameScreen();
        WinScreen*      winScreen  = new WinScreen();

        // ── Load config ───────────────────────────────────────────────────
        std::cerr << "Loading config..." << std::endl;
        ConfigLoader config("../config");
        auto miscCfg    = config.loadMisc();
        auto specialCfg = config.loadSpecial();
        auto propData   = config.loadProperties();
        auto rrData     = config.loadRailroad();
        auto utilData   = config.loadUtility();
        std::cerr << "Config loaded! Properties: " << propData.size() << std::endl;

        // ── Entitas dasar ─────────────────────────────────────────────────
        Bank*              bank   = new Bank();
        Dice*              dice   = new Dice();
        AuctionManager*    am     = new AuctionManager();
        TransactionLogger* logger = new TransactionLogger();

        // ── Deck kartu ────────────────────────────────────────────────────
        CardDeck<Card>* chanceDeck    = DeckFactory::createChanceDeck();
        CardDeck<Card>* communityDeck = DeckFactory::createCommunityDeck();
        CardDeck<Card>* skillDeck     = DeckFactory::createSkillDeck();
        std::cerr << "Decks created!" << std::endl;

        // ── Properti ──────────────────────────────────────────────────────
        auto properties = PropertyFactory::createProperties(propData, rrData, utilData);
        std::cerr << "Properties created: " << properties.size() << std::endl;

        // ── Board (minimal — tambah semua 40 tile nanti) ──────────────────
        std::vector<Tile*> tiles;
        tiles.push_back(new GoTile(0, "GO", TileType::SPECIAL,
            TileName::GO, "GO", specialCfg.goSalary));
        // TODO: tambah semua 40 tile, link property via setProperty()
        Board* board = new Board(tiles, tiles.size());

        // ── GameState ─────────────────────────────────────────────────────
        std::vector<Player*> players;
        GameState state(
            miscCfg.maxTurn,
            players,
            board,
            bank,
            dice,
            am,
            chanceDeck,
            communityDeck,
            skillDeck,
            logger
        );
        std::cerr << "GameState created!" << std::endl;

        // ── GameMaster ────────────────────────────────────────────────────
        GameMaster* gm = new GameMaster(state);
        gameScreen->setGameMaster(gm);
        std::cerr << "GameMaster created!" << std::endl;

        // ── Mulai dari main menu ──────────────────────────────────────────
        gui.setScreen(menuScreen);

        while (!WindowShouldClose()) {
            float dt = GetFrameTime();

            // MainMenu → GameScreen
            if (menuScreen->isReadyToStart()) {
                auto setup = menuScreen->getSetup();
                gameScreen->setPlayerCount(setup.playerCount);
                gameScreen->setPlayerNames(setup.names);

                // Buat Player dari username
                for (int i = 0; i < setup.playerCount; i++) {
                    Player* p = new Player(setup.names[i], miscCfg.initialBalance);
                    p->setPosition(0);
                    gm->getState().addPlayer(p);
                }

                menuScreen->resetReady();
                // gm->beginTurn(); // uncomment kalau Board sudah lengkap
                gui.setScreen(gameScreen);
            }

            // GameScreen → WinScreen
            if (gameScreen->isGameOver()) {
                winScreen->setResults(
                    gameScreen->getResults(),
                    gameScreen->getScenario()
                );
                gameScreen->gameOver = false;
                gui.setScreen(winScreen);
            }

            // WinScreen → MainMenu / Exit
            if (winScreen->goToMainMenu()) {
                winScreen->reset();
                gui.setScreen(menuScreen);
            }
            if (winScreen->goToExit()) break;

            // Update + Render
            BeginDrawing();
            IScreen* cur = gui.getCurrentScreen();
            if (cur) {
                cur->update(dt);
                cur->render(gui.getWindow());
            }
            EndDrawing();
        }

        gui.setScreen(nullptr);

        // Cleanup
        DeckFactory::deleteDeckCards(chanceDeck);
        DeckFactory::deleteDeckCards(communityDeck);
        DeckFactory::deleteDeckCards(skillDeck);
        delete chanceDeck;
        delete communityDeck;
        delete skillDeck;
        delete gm;
        delete bank;
        delete dice;
        delete am;
        delete logger;
        delete menuScreen;
        delete gameScreen;
        delete winScreen;

    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        CloseWindow();
        return 1;
    }

    CloseWindow();
    return 0;
}