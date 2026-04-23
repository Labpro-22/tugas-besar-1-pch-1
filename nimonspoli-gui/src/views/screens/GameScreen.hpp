#pragma once
#include "../../../lib/raylib/include/raylib.h"
#include "../IScreen.hpp"
#include "../../core/utils/TransactionLogger.hpp"
#include "../GUIManager.hpp"

#include <string>
#include <vector>
#include <map>

// ─── Mock data structs (replace with real backend later) ───────────────────
struct MockProperty {
    std::string code;
    std::string name;
    std::string colorGroup;
    std::string type;         // "STREET","RAILROAD","UTILITY","ACTION"
    int         owner;        // -1=bank, 0-3=player index
    int         buildings;    // 0-4 houses, 5=hotel
    bool        mortgaged;
    int         festivalMult; // 1,2,4,8
    int         festivalDur;
    int rentL0,rentL1,rentL2,rentL3,rentL4,rentL5;
    int price, mortgageVal, houseUpg, hotelUpg;
};

struct MockPlayer {
    std::string username;
    int         money;
    int         position;    // tile render index 0-39
    std::string status;      // "ACTIVE","JAILED","BANKRUPT"
    int         cardCount;
    bool        isCurrentTurn;
};

struct MockGameState {
    int currentTurn;
    int maxTurn;
    int activePlayerIdx;
    std::vector<MockPlayer>   players;
    std::vector<MockProperty> properties;
    TransactionLogger logger;
    bool kspGlow;
    bool dnuGlow;
};

struct TileDef {
    std::string code;
    std::string side;   // "BOTTOM","LEFT","TOP","RIGHT"
    bool        corner;
};

// ─── GameScreen : public IScreen ──────────────────────────────────────────
class GameScreen : public IScreen {
public:
    GameScreen();
    ~GameScreen() override;

    
    void onEnter() override;
    void onExit()  override;
    void update(float dt) override;
    void render(Window& window) override;
    void setPlayerCount(int n) { activePlayerCount = n; }
    void setGUIManager(GUIManager* gm) { guiManager = gm; }  // inject GUIManager
    int  activePlayerCount = 4;
    bool gameOver = false;
    bool isGameOver() const { return gameOver; }
    // Methods untuk sinkronisasi dengan GameMaster (mode real)
    void    syncFromGameMaster(); 
    bool    isRealMode() const;
    void    syncDiceResult(); 

    // ── Player colors ────────────────────────────────────────────────────
    Color playerColors[4] = {
        {220,  50,  50, 255},  // P1 red
        {240, 200,  50, 255},  // P2 yellow
        { 50, 180,  50, 255},  // P3 green
        { 50, 200, 220, 255},  // P4 cyan
    };

private:

    // ── Save Tools ────────────
    struct SavePopup {
        bool    visible     = false;
        bool    confirmOverwrite = false;   // true = sedang tanya "timpa?"
        bool    resultVisible = false;      // true = tampilkan pesan hasil
        bool    resultOk    = false;
        float   resultTimer = 0.f;
        std::string fileNameInput = "save"; // buffer nama file tanpa ekstensi
        std::string resultMsg;
    } savePopup;
 
    void handleSimpan();
    void drawSavePopup();
    void doSave(const std::string& filepath);
    

    // ── Layout ──────────────────────────────────────────────────────────
    static constexpr int   SCREEN_W    = 1920;
    static constexpr int   SCREEN_H    = 1080;
    static constexpr int   LEFT_PANEL  = 270;
    static constexpr int   RIGHT_PANEL = 270;

    static constexpr float ORIG_TILE_W  = 200.f;
    static constexpr float ORIG_TILE_H  = 290.f;
    static constexpr float ORIG_CORNER  = 290.f;
    static constexpr float ORIG_STRIP_W = 200.f;
    static constexpr float ORIG_STRIP_H =  35.f;

    // Scale: board height = 2*290 + 9*200 = 2380, fit to 1080
    static constexpr float SCALE      = 1080.f / 2380.f;
    static constexpr float TILE_W     = ORIG_TILE_W  * SCALE;
    static constexpr float TILE_H     = ORIG_TILE_H  * SCALE;
    static constexpr float CORNER_SZ  = ORIG_CORNER  * SCALE;
    static constexpr float STRIP_W    = ORIG_STRIP_W * SCALE;
    static constexpr float STRIP_H    = ORIG_STRIP_H * SCALE;

    // TransactionLogger: Popup-screen attributes
    bool showLogPopup;
    int logShowN;
    float logScrollY;

    void initMockLogs(); /* Untuk Load Data Logs palsu*/
    void drawLogPopup();
    std::string getActionIcon(const std::string& action);
    Color getActionColor(const std::string& action);
    // Hanlde keyboardInput
    std::string logNInput;   // string yang diketik user
    bool logNFocused; // apakah input box sedang aktif
    
    
    float boardX, boardY;

    // ── Tiles ────────────────────────────────────────────────────────────
    std::vector<TileDef> tiles;

    // ── Textures ─────────────────────────────────────────────────────────
    std::map<std::string, Texture2D> tileTextures;
    Texture2D deckKSP{}, deckDNU{};

    // ── Zoom (center only) ───────────────────────────────────────────────
    float   zoomLevel;
    Vector2 zoomOffset;
    bool    isDragging;
    Vector2 dragStart;

    // ── Popup ────────────────────────────────────────────────────────────
    int  selectedTile;
    bool showPopup;

    // ── Glow ─────────────────────────────────────────────────────────────
    bool  kspGlowing, dnuGlowing;
    float glowTimer;

    // ── State ────────────────────────────────────────────────────────────
    MockGameState gameState;

    // ── GUIManager (untuk pushCommand) ───────────────────────────────────
    GUIManager* guiManager = nullptr;

    // ── State dadu ───────────────────────────────────────────────────────
    struct DiceState {
        int val1 = 0, val2 = 0;    // hasil terakhir (0 = belum pernah lempar)
        bool hasRolled  = false;    // sudah lempar giliran ini?
        bool isDouble   = false;    // hasil double?
        bool tripleDouble = false;  // 3× double → penjara
        float animTimer = 0.f;     // timer animasi rolling (0 = idle)
        bool  animating = false;
        static constexpr float ANIM_DURATION = 0.6f; // detik
    } diceState;

    void drawDiceArea();            // gambar dadu di center board
    void handleLemparDadu();        // buat & push LemparDaduCommand

    // ── State dialog beli ────────────────────────────────────────────────
    struct BuyDialogState {
        bool visible   = false;
        int  tileIdx   = -1;   // indeks petak properti yang diinjak
        bool canAfford = true; // apakah pemain mampu bayar?
    } buyDialog;

    void drawBuyDialog();    // render dialog beli/skip
    void triggerBuyDialog(int tileIdx); // dipanggil setelah dadu mendarat di properti BANK

    

    // ── Private methods ──────────────────────────────────────────────────
    Color   getGroupColor(const std::string& group);
    void    loadTextures();
    void    initMockState();
     
    void    handleInput();
    void    drawBoard();
    void    drawTile(int idx, float cx, float cy, float rotation);
    void    drawBuildingStrip(float cx, float cy, float rotation,
                              int buildings, Color ownerColor);
    void    drawPlayers(int tileIdx, float cx, float cy);
    void    drawCenterArea();
    void    drawLeftPanel();
    void    drawRightPanel();
    void    drawPopup();

    Vector2   getTileCenter(int idx);
    Rectangle getTileRect(int idx);
    int       tileAtPoint(Vector2 pt);

    

    

};