#include "../lib/raylib/include/raylib.h"
#include "views/GUIManager.hpp"
#include "views/screens/GameScreen.hpp"
#include "views/screens/MainMenuScreen.hpp"
#include "views/screens/WinScreen.hpp"
#include "core/Commands/BeliCommand.hpp"

#include "core/GameMaster/GameMaster.hpp"
#include "core/GameState/GameState.hpp"
#include "core/Board/Board.hpp"
#include "core/Player/Player.hpp"
#include "core/ComputerPlayer/ComputerPlayer.hpp"
#include "core/Bank/Bank.hpp"
#include "core/Dice/Dice.hpp"
#include "core/AuctionManager/AuctionManager.hpp"
#include "core/utils/TransactionLogger.hpp"
#include "core/utils/ConfigLoader.hpp"
#include "core/Property/PropertyFactory.hpp"
#include "core/Board/BoardFactory.hpp"
#include "core/utils/SaveLoadManager.hpp"
#include "core/Commands/BankruptCommand.hpp"
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

#include "core/Card/CardDeck.hpp"
#include "core/Card/Card.hpp"
#include "core/Card/SkillCard.hpp"
#include "core/Card/MoveCard.hpp"
#include "core/Card/DiscountCard.hpp"
#include "core/Card/ShieldCard.hpp"
#include "core/Card/TeleportCard.hpp"
#include "core/Card/LassoCard.hpp"
#include "core/Card/DemolitionCard.hpp"
#include "core/Card/DeckFactory.hpp"

class PlayerInfo
{
public:
    std::string name;
    bool isBot;
    COMDifficulty diff;
};


int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    std::string title = "Nimonspoli";
    GUIManager gui(1920, 1080, title, 60);
    MainMenuScreen *menuScreen = new MainMenuScreen();
    GameScreen *gameScreen = new GameScreen();
    WinScreen *winScreen = new WinScreen();

    gui.setScreen(menuScreen);

    GameMaster *gameMaster = nullptr;
    Bank *bank = nullptr;
    Dice *dice = nullptr;
    AuctionManager *auctionMgr = nullptr;
    CardDeck<Card> *chanceDeck = nullptr;
    CardDeck<Card> *communityDeck = nullptr;
    CardDeck<SkillCard> *skillDeck = nullptr;
    TransactionLogger *logger = nullptr;
    Board *board = nullptr;
    std::vector<Player *> players;
    std::vector<std::unique_ptr<Property>> properties;

    auto cleanupGame = [&]()
    {
        delete gameMaster;
        gameMaster = nullptr;
        delete bank;
        bank = nullptr;
        delete dice;
        dice = nullptr;
        delete auctionMgr;
        auctionMgr = nullptr;
        delete chanceDeck;
        chanceDeck = nullptr;
        delete communityDeck;
        communityDeck = nullptr;
        delete skillDeck;
        skillDeck = nullptr;
        delete logger;
        logger = nullptr;
        delete board;
        board = nullptr;
        for (auto *p : players)
            delete p;
        players.clear();
        properties.clear();
    };

    bool comHasActed = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ── Transisi: Menu → Game ─────────────────────────────────────────
        if (menuScreen->isReadyToStart())
        {
            auto setup = menuScreen->getSetup();
            menuScreen->resetReady();
            cleanupGame();

            ConfigLoader cfg("config");
            auto propData = cfg.loadProperties();
            auto railroadRent = cfg.loadRailroad();
            auto utilityFactor = cfg.loadUtility();
            auto specialCfg = cfg.loadSpecial();
            auto miscCfg = cfg.loadMisc();
            auto actData = cfg.loadActions();
            auto taxData = cfg.loadTax();

            properties = PropertyFactory::createProperties(propData, railroadRent, utilityFactor);
            bank = new Bank(32, 12);
            dice = new Dice();
            auctionMgr = new AuctionManager();
            logger = new TransactionLogger();
            chanceDeck = DeckFactory::createChanceDeck();
            communityDeck = DeckFactory::createCommunityDeck();
            skillDeck = DeckFactory::createSkillDeck();
            board = BoardFactory::createBoard(propData, actData, specialCfg,
                                              chanceDeck, communityDeck, properties);

            players.clear();

            if (setup.isLoadGame)
            {
                std::vector<PlayerInfo> loadedInfos;
                {
                    std::ifstream peek(setup.saveFile);
                    if (peek.is_open()) {
                        std::string line;
                        std::getline(peek, line); // SKIP TURN
                        std::getline(peek, line); // PLAYER COUNT
                        if (!line.empty()) {
                            int savedCount = std::stoi(line);
                            for (int i = 0; i < savedCount; i++)
                            {
                                if (!std::getline(peek, line)) break;
                                std::vector<std::string> tok;
                                std::istringstream ss(line);
                                std::string w;
                                while (ss >> w) tok.push_back(w);
                                
                                PlayerInfo info;
                                info.name = tok.size() > 0 ? tok[0] : "_tmp_";
                                info.isBot = tok.size() > 4 && tok[4] == "COM";
                                info.diff = COMDifficulty::MEDIUM;
                                if (tok.size() > 5)
                                {
                                    if (tok[5] == "EASY") info.diff = COMDifficulty::EASY;
                                    if (tok[5] == "HARD") info.diff = COMDifficulty::HARD;
                                }
                                loadedInfos.push_back(info);
                                
                                if (!std::getline(peek, line)) break;
                                int cardCount = line.empty() ? 0 : std::stoi(line);
                                for (int c = 0; c < cardCount; c++)
                                    std::getline(peek, line);
                            }
                        }
                    }
                }
                for (auto &info : loadedInfos)
                {
                    Player *p = info.isBot
                                    ? new ComputerPlayer(info.name, miscCfg.initialBalance, info.diff)
                                    : new Player(info.name, miscCfg.initialBalance);
                    p->setPosition(0);
                    players.push_back(p);
                }
                
                gameMaster = new GameMaster(GameState(miscCfg.maxTurn, players, board, bank, dice,
                                                      auctionMgr, chanceDeck, communityDeck, skillDeck, logger, taxData));
                try
                {
                    SaveLoadManager slm;
                    slm.load(setup.saveFile, gameMaster->getState());
                    gameMaster->log("SYSTEM", "MUAT", "Game_dimuat_dari_" + setup.saveFile); 
                    gameMaster->beginTurn();
                }
                catch (const std::exception &e)
                {
                    std::cerr << "[MUAT] Gagal: " << e.what() << std::endl;
                }
            }
            else
            {
                int count = std::max(2, std::min(4, setup.playerCount));
                for (int i = 0; i < count; ++i)
                {
                    std::string name = (i < (int)setup.names.size() && !setup.names[i].empty())
                                           ? setup.names[i]
                                           : ("Pemain" + std::to_string(i + 1));
                    Player *p = setup.isBot[i]
                                    ? new ComputerPlayer(name, miscCfg.initialBalance, setup.botDifficulty)
                                    : new Player(name, miscCfg.initialBalance);
                    p->setPosition(0);
                    players.push_back(p);
                }
                
                gameMaster = new GameMaster(GameState(miscCfg.maxTurn, players, board, bank, dice,
                                                      auctionMgr, chanceDeck, communityDeck, skillDeck, logger, taxData));
                gameMaster->beginTurn();
            }

            gui.setGameMaster(gameMaster);
            gameScreen->setGUIManager(&gui);
            gameScreen->setPlayerCount((int)players.size());
            gui.setScreen(gameScreen);
            comHasActed = false;
        }

        // ── Transisi: Game → Win ──────────────────────────────────────────
        if (gameScreen->isGameOver()) {
            std::vector<PlayerResult> results;
            auto& gs = gameMaster->getState();
            
            for (int i = 0; i < (int)players.size(); ++i) {
                Player* p = players[i];
                PlayerResult r;
                r.username      = p->getUsername();
                r.money         = p->getBalance();
                r.propertyCount = p->getPropertyCount();
                r.cardCount     = p->getHandSize();
                r.bankrupt      = (p->getStatus() == PlayerStatus::BANKRUPT);
                r.rank          = 0;
                r.isWinner      = false;
                r.color         = gameScreen->playerColors[i];
                results.push_back(r);
            }
            
            // Tentukan scenario
            int activePlayers = gs.countActivePlayers();
            WinScenario sc = (activePlayers <= 1) 
                            ? WinScenario::BANKRUPTCY 
                            : WinScenario::MAX_TURN;
            
            winScreen->setResults(results, sc);
            gameScreen->gameOver = false;
            gui.setGameMaster(nullptr);
            gui.setScreen(winScreen);
        }

        // ── Transisi: Win → Menu ──────────────────────────────────────────
        if (winScreen->goToMainMenu())
        {
            winScreen->reset();
            cleanupGame();
            gui.setScreen(menuScreen);
        }
        if (winScreen->goToExit())
            break;

        // ── Transisi: Game → Menu ─────────────────────────────────────────
        if (gameScreen->isWantsMainMenu())
        {
            gameScreen->resetWantsMainMenu();
            cleanupGame();
            gui.setScreen(menuScreen);
        }

        // ── COM auto-play ─────────────────────────────────────────────────
        if (gameMaster)
        {
            GameState &state = gameMaster->getState();
            Player *curr = state.getCurrPlayer();
            ComputerPlayer *com = dynamic_cast<ComputerPlayer *>(curr);

            
                // Tambah setelah blok BANKRUPTCY handling:

            // ── COM drop skill card ───────────────────────────────────────────
            if (com && state.getPhase() == GamePhase::AWAITING_DROP_SKILL_CARD) {
                Player* dropPlayer = state.getPendingSkillDropPlayer();
                // Pastikan yang harus drop adalah COM ini
                if (dropPlayer == com) {
                    // COM pilih kartu yang dibuang
                    // Kartu baru belum masuk tangan, tapi hand sudah penuh
                    // decideDropCard dipanggil dengan hand saat ini
                    int dropIdx = com->decideDropCard(com->getHand(), nullptr);
                    
                    // Validasi index
                    if (dropIdx >= 0 && dropIdx < com->getHandSize()) {
                        SkillCard* removed = com->discardSkillCard(dropIdx);
                        if (removed) {
                            if (state.getSkillDeck()) {
                                state.getSkillDeck()->discard(removed);
                            }
                            gameMaster->log(com->getUsername(), "DROP_CARD",
                                "Bot membuang kartu: " + removed->getType());
                        }
                        state.clearPendingSkillDrop();
                        state.setPhase(GamePhase::PLAYER_TURN);
                    }
                }
            }

            if (com && state.getPhase() == GamePhase::PLAYER_TURN && !state.getHasRolled() && !comHasActed)
            {
                comHasActed = true;
                com->executeTurn(*gameMaster);
                GamePhase p = state.getPhase();
                if (p == GamePhase::PLAYER_TURN)
                {
                    gameMaster->endTurn();
                    gameMaster->beginTurn();
                }
                gui.clearCommands();
            }

            if (com && state.getPhase() == GamePhase::BANKRUPTCY)
            {
                Player *creditor = state.getPendingCreditor();
                int debt = state.getPendingDebt();

                while (com->getBalance() < debt && com->getPropertyCount() > 0 && com->getWealth() >= debt)
                {
                    Property *propToSell = nullptr;
                    for (int pi = 0; pi < com->getPropertyCount(); ++pi)
                    {
                        Property *candidate = com->getProperties()[pi];
                        if (candidate && candidate->getStatus() != PropertyStatus::MORTGAGED)
                        {
                            propToSell = candidate;
                            break;
                        }
                    }
                    if (!propToSell) break;
                    gameMaster->sellPropertyToBank(com, propToSell);
                }

                if (com->getBalance() >= debt)
                {
                    if (creditor) { *com -= debt; *creditor += debt; }
                    else { *com -= debt; }
                    gameMaster->log(com->getUsername(), "BAYAR_HUTANG_BOT",
                                    "Bot membayar M" + std::to_string(debt));
                    state.setPendingDebt(0);
                    state.setPendingCreditor(nullptr);
                    state.setPhase(GamePhase::PLAYER_TURN);
                }
                else
                {
                    if (creditor) gameMaster->handleBankruptcy(com, creditor);
                    else gameMaster->handleBankruptcy(com, state.getBank());
                }

            }

            if (!com || !state.getHasRolled())
                comHasActed = false;
        }

        BeginDrawing();
        if (gui.getCurrentScreen())
        {
            gui.flushCommands();
            gui.getCurrentScreen()->update(dt);
            gui.getCurrentScreen()->render(gui.getWindow());
        }
        EndDrawing();

        static int frameCount = 0;
        if (++frameCount % 60 == 0)
            TraceLog(LOG_INFO, "Frame %d", frameCount);
    }

    cleanupGame();
    gui.setScreen(nullptr);
    delete menuScreen;
    delete gameScreen;
    delete winScreen;
    return 0;
}