#pragma once
#include "../../../lib/raylib/include/raylib.h"
#include "../IScreen.hpp"
#include "../../core/utils/TransactionLogger.hpp"
#include "../GUIManager.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Card/SkillCard.hpp"
#include "../../core/Card/MoveCard.hpp"
#include "../../core/Card/DiscountCard.hpp"
#include "../../core/Card/ShieldCard.hpp"
#include "../../core/Card/TeleportCard.hpp"
#include "../../core/Card/LassoCard.hpp"
#include "../../core/Card/DemolitionCard.hpp"

#include <string>
#include <vector>
#include <map>

class TileDef
{
public:
    std::string code;
    std::string side;
    bool corner = false;
};

// ─── GameScreen : public IScreen ──────────────────────────────────────────
class GameScreen : public IScreen
{
public:
    GameScreen();
    ~GameScreen() override;

    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void render(Window &window) override;

    void setPlayerCount(int n) { activePlayerCount = n; }
    void setGUIManager(GUIManager *gm) { guiManager = gm; }

    int activePlayerCount = 4;
    bool gameOver = false;
    bool pendingTileEffectAfterCard = false;
    bool isGameOver() const { return gameOver; }

    // Sinkronisasi dengan GameMaster (mode real)
    void syncFromGameMaster();
    bool isRealMode() const;
    void syncDiceResult();

    // ── Player colors ────────────────────────────────────────────────────
    Color playerColors[4] = {
        {220, 50, 50, 255},  // P1 red
        {240, 200, 50, 255}, // P2 yellow
        {50, 180, 50, 255},  // P3 green
        {50, 200, 220, 255}, // P4 cyan
    };


    bool isWantsMainMenu() const { return wantsMainMenu; }
    void resetWantsMainMenu() { wantsMainMenu = false; }
    bool wantsMainMenu = false;
private:
    // ════════════════════════════════════════════════════════════════
    //  Nested classes — pengganti semua struct
    // ════════════════════════════════════════════════════════════════

    // ── Mock data ────────────────────────────────────────────────────────
    class MockProperty
    {
    public:
        std::string code, name, colorGroup, type;
        int owner = -1;
        int buildings = 0;
        bool mortgaged = false;
        int festivalMult = 1;
        int festivalDur = 0;
        int rentL0 = 0, rentL1 = 0, rentL2 = 0,
            rentL3 = 0, rentL4 = 0, rentL5 = 0;
        int price = 0, mortgageVal = 0, houseUpg = 0, hotelUpg = 0;
    };

    class MockPlayer {
    public:
        std::string username;
        int  money         = 0;
        int  position      = 0;
        std::string status;
        int  cardCount     = 0;
        bool isCurrentTurn = false;
    };

    class MockGameState
    {
    public:
        int currentTurn = 0;
        int maxTurn = 0;
        int activePlayerIdx = 0;
        std::vector<MockPlayer> players;
        std::vector<MockProperty> properties;
        TransactionLogger logger;
        bool kspGlow = false;
        bool dnuGlow = false;
    };

    struct MainMenuConfirmState {
        bool visible = false;
        bool wantSave = false;
    } mainMenuConfirm;


    void drawMainMenuConfirm();

    // ── Player Visual ────────────────────────────────────────────────────
    class PlayerVisual
    {
    public:
        float currentTileIdx = 0.f;
        float targetTileIdx = 0.f;
    };

    // ── Dice State ───────────────────────────────────────────────────────
    class DiceState
    {
    public:
        int val1 = 0;
        int val2 = 0;
        bool hasRolled = false;
        bool isDouble = false;
        bool tripleDouble = false;
        float animTimer = 0.f;
        bool animating = false;
        static constexpr float ANIM_DURATION = 0.6f;
    };

    // ── Save Popup ────────────────────────────────────────────────────────
    class SavePopup
    {
    public:
        bool visible = false;
        bool confirmOverwrite = false;
        bool resultVisible = false;
        bool resultOk = false;
        float resultTimer = 0.f;
        std::string fileNameInput = "save";
        std::string resultMsg;
    };

    // ── Buy Dialog ────────────────────────────────────────────────────────
    class BuyDialogState
    {
    public:
        bool visible = false;
        int tileIdx = -1;
        bool canAfford = true;
    };

    // ── Tax Dialog ────────────────────────────────────────────────────────
    class TaxDialogState
    {
    public:
        bool visible = false;
        int flatAmount = 0;
        int pctAmount = 0;
        int wealth = 0;
        bool canAffordFlat = true;
        bool canAffordPct = true;
        int taxAmtPct = 0;
    };

    // ── PBM Dialog ────────────────────────────────────────────────────────
    class PbmDialogState
    {
    public:
        bool visible = false;
        int amount = 0;
        int balanceBefore = 0;
        int balanceAfter = 0;
    };

    // ── Festival Dialog ───────────────────────────────────────────────────
    class FestivalDialogState
    {
    public:
        bool visible = false;
        float scrollY = 0.f;
        std::vector<StreetProperty *> streets;
        int hoveredIdx = -1;
    };

    // ── Card Dialog ───────────────────────────────────────────────────────
    class CardDialogState
    {
    public:
        bool visible = false;
        std::string deckLabel;
        std::string description;
    };

    // ── Jail Dialog ───────────────────────────────────────────────────────
    class JailDialog
    {
    public:
        bool visible = false;
        int jailFine = 50;
        bool canAffordFine = true;
        int jailTurnsLeft = 0;
        bool forcedPay = false;
        bool rolledThisTurn = false;
    };

    // ── Auction Dialog ────────────────────────────────────────────────────
    class AuctionDialogState
    {
    public:
        bool visible = false;
        std::string propertyName;
        std::string propertyGroup;
        int currentBid = 0;
        std::string highestBidder;
        std::string currParticipant;
        bool isForcedBid = false;
        std::string bidInput;
        bool bidFocused = false;
    };

    // ── Properti Popup ────────────────────────────────────────────────────
    class PropertiPopupState
    {
    public:
        bool visible = false;
        float scrollY = 0.f;
    };

    // ── Akta Dialog ───────────────────────────────────────────────────────
    class AktaDialogState
    {
    public:
        bool visible = false;
        bool inputFocused = true;
        float scrollY = 0.f;
        std::string inputCode;
        std::string content;
        std::string propName;
        std::string errorMsg;
    };

    // ── Cetak Properti Dialog ─────────────────────────────────────────────
    class CetakPropertiDialogState
    {
    public:
        bool visible = false;
        float scrollY = 0.f;
        std::string content;
        std::string playerName;
        int playerIdx = -1;
    };

    // ── Dialog Gadai ──────────────────────────────────────────────────────
    class GadaiDialogState
    {
    public:
        class Entry
        {
        public:
            Property *prop = nullptr;
            int tileIdx = -1;
        };
        bool visible = false;
        float scrollY = 0.f;
        std::vector<Entry> entries;
        int hoveredIdx = -1;
    };

    // ── Dialog Tebus ──────────────────────────────────────────────────────
    class TebusDialogState
    {
    public:
        class Entry
        {
        public:
            Property *prop = nullptr;
            int tileIdx = -1;
        };
        bool visible = false;
        float scrollY = 0.f;
        std::vector<Entry> entries;
        int hoveredIdx = -1;
    };

    // ── Dialog Bangun ─────────────────────────────────────────────────────
    class BangunDialogState
    {
    public:
        bool visible = false;
        float scrollY = 0.f;
        int selectedGroupIdx = -1;
        std::vector<std::pair<std::string, std::vector<StreetProperty *>>> groups;
        int hoveredIdx = -1;
    };

    // ── Dialog Jual Bangunan ──────────────────────────────────────────────
    class JualBangunanDialogState
    {
    public:
        class Entry
        {
        public:
            StreetProperty *prop = nullptr;
            int tileIdx = -1;
        };
        bool visible = false;
        float scrollY = 0.f;
        std::vector<Entry> entries;
        int hoveredIdx = -1;
    };

    // ── Dialog Atur Dadu ──────────────────────────────────────────────────
    class AturDaduDialogState
    {
    public:
        bool visible = false;
        std::string input1 = "1";
        std::string input2 = "1";
        int focusField = 0;
        std::string errorMsg;
    };

    // ── Dialog Skill Card ─────────────────────────────────────────────────
    class SkillCardDialogState
    {
    public:
        bool visible = false;
        int hoveredIdx = -1;
        bool awaitingTeleportTile = false;
        bool awaitingLassoTarget = false;
        int selectedCardIdx = -1;
    };

    class SkillCardResultDialogState
    {
    public:
        bool visible = false;
        std::string title;
        std::string message;
    };

    class SkillTargetHintState
    {
    public:
        bool visible = false;
        std::string message;
    };

    class LassoTargetDialogState
    {
    public:
        bool visible = false;
        int selectedCardIdx = -1;
        std::string errorMsg;
    };

    class DemolitionTargetDialogState
    {
    public:
        bool visible = false;
        int selectedCardIdx = -1;
        std::string errorMsg;
    };

    class DropSkillCardDialogState
    {
    public:
        bool visible = false;
        int hoveredIdx = -1;
        std::string errorMsg;
    };

    // ── Dialog Bankruptcy ─────────────────────────────────────────────────
    class BankruptcyDialogState
    {
    public:
        class Entry
        {
        public:
            Property *prop = nullptr;
            int tileIdx = -1;
            bool canSell = false;
            bool canMortgage = false;
            int sellValue = 0;
            int mortgageValue = 0;
        };
        bool visible = false;
        bool notifVisible = false;
        float scrollY = 0.f;
        int pendingDebt = 0;
        std::string creditorName;
        std::vector<Entry> entries;
        int hoveredSellIdx = -1;
        int hoveredMortgageIdx = -1;
        std::string bankruptName;
    };

    // ── Dialog Sewa ───────────────────────────────────────────────────────
    class SewaDialogState
    {
    public:
        bool visible = false;
        std::string tileName;
        std::string ownerName;
        std::string conditionStr;
        int rentAmount = 0;
        int landerBalBefore = 0;
        int ownerBalBefore = 0;
        bool canAfford = true;
    };

    // ════════════════════════════════════════════════════════════════
    //  Layout constants
    // ════════════════════════════════════════════════════════════════
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;
    static constexpr int LEFT_PANEL = 270;
    static constexpr int RIGHT_PANEL = 270;

    static constexpr float ORIG_TILE_W = 200.f;
    static constexpr float ORIG_TILE_H = 290.f;
    static constexpr float ORIG_CORNER = 290.f;
    static constexpr float ORIG_STRIP_W = 200.f;
    static constexpr float ORIG_STRIP_H = 35.f;

    static constexpr float SCALE = 1080.f / 2380.f;
    static constexpr float TILE_W = ORIG_TILE_W * SCALE;
    static constexpr float TILE_H = ORIG_TILE_H * SCALE;
    static constexpr float CORNER_SZ = ORIG_CORNER * SCALE;
    static constexpr float STRIP_W = ORIG_STRIP_W * SCALE;
    static constexpr float STRIP_H = ORIG_STRIP_H * SCALE;

    float boardX = 0.f, boardY = 0.f;

    // ════════════════════════════════════════════════════════════════
    //  Atribut
    // ════════════════════════════════════════════════════════════════
    std::vector<TileDef> tiles;
    std::map<std::string, Texture2D> tileTextures;
    Texture2D deckKSP{}, deckDNU{};
    std::map<std::string, Texture2D> cardTextures;

    float zoomLevel = 1.f;
    Vector2 zoomOffset = {};
    bool isDragging = false;
    Vector2 dragStart = {};

    int selectedTile = -1;
    bool showPopup = false;

    bool kspGlowing = false, dnuGlowing = false;
    float glowTimer = 0.f;

    MockGameState gameState;
    GUIManager *guiManager = nullptr;

    std::vector<PlayerVisual> playerVisuals;

    // ── Instansiasi semua dialog ──────────────────────────────────────────
    DiceState diceState;
    SavePopup savePopup;
    BuyDialogState buyDialog;
    TaxDialogState taxDialog;
    PbmDialogState pbmDialog;
    FestivalDialogState festivalDialog;
    CardDialogState cardDialog;
    JailDialog jailDialog;
    AuctionDialogState auctionDialog;
    PropertiPopupState propertiPopup;
    AktaDialogState aktaDialog;
    CetakPropertiDialogState cetakPropertiDialog;
    GadaiDialogState gadaiDialog;
    TebusDialogState tebusDialog;
    BangunDialogState bangunDialog;
    JualBangunanDialogState jualBangunanDialog;
    AturDaduDialogState aturDaduDialog;
    SkillCardDialogState skillCardDialog;
    SkillCardResultDialogState skillCardResultDialog;
    SkillTargetHintState skillTargetHint;
    LassoTargetDialogState lassoTargetDialog;
    DemolitionTargetDialogState demolitionTargetDialog;
    DropSkillCardDialogState dropSkillCardDialog;
    BankruptcyDialogState bankruptcyDialog;
    SewaDialogState sewaDialog;

    // ── Log Popup ─────────────────────────────────────────────────────────
    bool showLogPopup = false;
    int logShowN = 10;
    float logScrollY = 0.f;
    std::string logNInput;
    bool logNFocused = false;

    // ════════════════════════════════════════════════════════════════
    //  Private methods
    // ════════════════════════════════════════════════════════════════
    Color getGroupColor(const std::string &group);
    void loadTextures();
    void initMockState();
    void handleInput();

    // ── GameScreenBoard.cpp ───────────────────────────────────────────────
    void drawBoard();
    void drawTile(int idx, float cx, float cy, float rotation);
    void drawBuildingStrip(float cx, float cy, float rotation,
                           int buildings, Color ownerColor);
    void drawPlayers(int tileIdx, float cx, float cy);
    void drawCenterArea();
    void drawPopup();
    Vector2 getTileCenter(int idx);
    Rectangle getTileRect(int idx);
    int tileAtPoint(Vector2 pt);

    // ── GameScreenPanels.cpp ──────────────────────────────────────────────
    void drawLeftPanel();
    void drawRightPanel();

    // ── GameScreenDice.cpp ────────────────────────────────────────────────
    void drawDiceArea();
    void handleLemparDadu();

    // ── GameScreenDialogBuy.cpp ───────────────────────────────────────────
    void triggerBuyDialog(int tileIdx);
    void drawBuyDialog();

    // ── GameScreenDialogTax.cpp ───────────────────────────────────────────
    void triggerTaxDialog();
    void drawTaxDialog();

    // ── GameScreenDialogPbm.cpp ───────────────────────────────────────────
    void triggerPbmDialog();
    void drawPbmDialog();

    // ── GameScreenDialogFestival.cpp ──────────────────────────────────────
    void triggerFestivalDialog();
    void drawFestivalDialog();

    // ── GameScreenDialogCard.cpp ──────────────────────────────────────────
    void triggerCardDialog();
    void drawCardDialog();

    // ── GameScreenDialogJail.cpp ──────────────────────────────────────────
    void triggerJailDialog();
    void drawJailDialog();

    // ── GameScreenDialogAuction.cpp ───────────────────────────────────────
    void triggerAuctionDialog();
    void drawAuctionDialog();

    // ── GameScreenDialogProperti.cpp ──────────────────────────────────────
    void drawPropertiPopup();

    // ── GameScreenDialogAkta.cpp ──────────────────────────────────────────
    void triggerAktaDialog();
    void drawAktaDialog();

    // ── GameScreenDialogCetakProperti.cpp ────────────────────────────────
    void triggerCetakPropertiDialog(int playerIdx);
    void drawCetakPropertiDialog();

    // ── GameScreenDialogGadai.cpp ─────────────────────────────────────────
    void triggerGadaiDialog();
    void drawGadaiDialog();

    // ── GameScreenDialogTebus.cpp ─────────────────────────────────────────
    void triggerTebusDialog();
    void drawTebusDialog();

    // ── GameScreenDialogBangun.cpp ────────────────────────────────────────
    void triggerBangunDialog();
    void drawBangunDialog();

    // ── GameScreenDialogJualBangunan.cpp ──────────────────────────────────
    void triggerJualBangunanDialog();
    void drawJualBangunanDialog();

    // ── GameScreenDialogAturDadu.cpp ──────────────────────────────────────
    void triggerAturDaduDialog();
    void drawAturDaduDialog();

    // ── GameScreenDialogSkillCard.cpp ─────────────────────────────────────
    void triggerSkillCardDialog();
    bool hasBlockingTileEffectDialog() const;
    void drawSkillCardDialog();
    void drawSkillCardResultDialog();
    void drawSkillTargetHint();
    void drawLassoTargetDialog();
    void drawDemolitionTargetDialog();
    void handleTeleportTargetClick(int selectedTile);
    void triggerDropSkillCardDialog();
    void drawDropSkillCardDialog();

    // ── GameScreenDialogBankruptcy.cpp ────────────────────────────────────
    void triggerBankruptcyDialog();
    void drawBankruptcyDialog();

    // ── GameScreenDialogSewa.cpp ──────────────────────────────────────────
    void triggerSewaDialog();
    void drawSewaDialog();

    // ── GameScreenSave.cpp ────────────────────────────────────────────────
    void handleSimpan();
    void drawSavePopup();
    void doSave(const std::string &filepath);

    // ── GameScreenLog.cpp ─────────────────────────────────────────────────
    void initMockLogs();
    void drawLogPopup();
    std::string getActionIcon(const std::string &action);
    Color getActionColor(const std::string &action);

    void searchAndFillAkta();
};