#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "raymath.h"
#include "../../core/Board/Board.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include <string>
#include <algorithm>

#include "GameScreenTiles.hpp"

// ─── Constructor ─────────────────────────────────────────────────────────────
GameScreen::GameScreen()
    : showLogPopup(false),
      logShowN(10),
      logScrollY(0.f),
      logNInput(""),
      logNFocused(false),
      zoomLevel(1.f),
      zoomOffset({0, 0}),
      isDragging(false),
      dragStart({0, 0}),
      selectedTile(-1),
      showPopup(false),
      kspGlowing(false),
      dnuGlowing(false),
      glowTimer(0.f)
{
    float boardTotalSz = CORNER_SZ + 9 * TILE_W + CORNER_SZ;
    float availW = SCREEN_W - LEFT_PANEL - RIGHT_PANEL;
    boardX = LEFT_PANEL + (availW - boardTotalSz) / 2.f;
    boardY = (SCREEN_H - boardTotalSz) / 2.f;
}

GameScreen::~GameScreen() {}

// ─── IScreen lifecycle ───────────────────────────────────────────────────────
void GameScreen::onEnter()
{
    tiles.clear();
    for (int i = 0; i < 40; i++)
        tiles.push_back(TILE_DEFS[i]);
    loadTextures();

    if (isRealMode())
    {
        TraceLog(LOG_INFO, "GameScreen: REAL MODE - players: %d",
                 (int)guiManager->getGameMaster()->getState().getPlayers().size());
        // Mode real: inisialisasi minimal, langsung sync dari GameMaster
        gameState.players.clear();
        gameState.properties.resize(40);
        for (int i = 0; i < 40; i++)
        {
            gameState.properties[i].code = TILE_DEFS[i].code;
            gameState.properties[i].owner = -1;
            gameState.properties[i].buildings = 0;
            gameState.properties[i].mortgaged = false;
            gameState.properties[i].festivalMult = 1;
            gameState.properties[i].festivalDur = 0;
            gameState.properties[i].type = "ACTION";
            gameState.properties[i].colorGroup = "";
        }
        playerVisuals.clear(); // penting! biar syncFromGameMaster inisialisasi ulang
        syncFromGameMaster();
    }
    else
    {
        TraceLog(LOG_WARNING, "GameScreen: MOCK MODE - guiManager=%s GM=%s",
                 guiManager ? "OK" : "NULL",
                 (guiManager && guiManager->getGameMaster()) ? "OK" : "NULL");
        // Mode mock: pakai data dummy untuk testing UI
        initMockState();
        initMockLogs();
    }
}

void GameScreen::onExit()
{
    for (auto &[k, v] : tileTextures)
        UnloadTexture(v);
    if (deckKSP.id != 0)
        UnloadTexture(deckKSP);
    if (deckDNU.id != 0)
        UnloadTexture(deckDNU);
    for (auto &[k, v] : cardTextures)
        UnloadTexture(v);
    cardTextures.clear();
    tiles.clear();
    tileTextures.clear();
}

bool GameScreen::isRealMode() const
{
    return guiManager != nullptr && guiManager->getGameMaster() != nullptr;
}

// ─── Update ──────────────────────────────────────────────────────────────────
void GameScreen::update(float dt)
{
    syncFromGameMaster();
    syncDiceResult();

    // Update Animasi Pion
    for (int i = 0; i < (int)playerVisuals.size(); i++)
    {
        auto &pv = playerVisuals[i];

        // Jika belum sampai di target tile
        if (pv.currentTileIdx != pv.targetTileIdx)
        {
            float step = 6.0f * dt; // Kecepatan gerak (5 tile per detik)

            // Logika berputar (handling modulo 40)
            float diff = pv.targetTileIdx - pv.currentTileIdx;
            if (diff < 0)
                diff += 40; // Selalu bergerak maju

            if (diff < step)
            {
                pv.currentTileIdx = pv.targetTileIdx;
            }
            else
            {
                pv.currentTileIdx += step;
                if (pv.currentTileIdx >= 40.0f)
                    pv.currentTileIdx -= 40.0f;
            }
        }
    }

    if (savePopup.resultTimer > 0)
        savePopup.resultTimer -= dt;
    if (diceState.animating)
    {
        diceState.animTimer += dt;
        if (diceState.animTimer >= DiceState::ANIM_DURATION)
            diceState.animating = false;
    }

    glowTimer += dt;
    handleInput();
}

// ─── Render ──────────────────────────────────────────────────────────────────
void GameScreen::render(Window &window)
{
    (void)window;
    ClearBackground({20, 22, 30, 255});
    drawLeftPanel();
    drawRightPanel();
    drawBoard();
    drawDiceArea();
    drawPopup();
    drawBuyDialog();
    drawAuctionDialog();
    drawTaxDialog();
    drawPbmDialog();
    drawFestivalDialog();
    drawSewaDialog();
    drawCardDialog();
    drawLogPopup();
    drawSavePopup();
    drawJailDialog();
    drawPropertiPopup();
    drawAturDaduDialog();

    drawGadaiDialog();
    drawTebusDialog();
    drawBangunDialog();
    drawJualBangunanDialog();
    drawSkillCardDialog();
    drawDropSkillCardDialog();

    drawSkillTargetHint();
    drawLassoTargetDialog();
    drawDemolitionTargetDialog();

    drawSkillCardResultDialog();
    drawBankruptcyDialog();
    drawAktaDialog();
    drawCetakPropertiDialog();
    drawMainMenuConfirm();

    DrawFPS(LEFT_PANEL + 4, 4);
}

// ─── Color helpers ────────────────────────────────────────────────────────────
Color GameScreen::getGroupColor(const std::string &g)
{
    if (g == "COKLAT")
        return {139, 69, 19, 255};
    if (g == "BIRU_MUDA")
        return {135, 206, 235, 255};
    if (g == "PINK" || g == "MERAH_MUDA")
        return {255, 105, 180, 255};
    if (g == "ORANGE")
        return {255, 165, 0, 255};
    if (g == "MERAH")
        return {220, 20, 60, 255};
    if (g == "KUNING")
        return {255, 215, 0, 255};
    if (g == "HIJAU")
        return {34, 139, 34, 255};
    if (g == "BIRU_TUA")
        return {0, 0, 139, 255};
    return GRAY;
}

// ─── Load textures ────────────────────────────────────────────────────────────
void GameScreen::loadTextures()
{
    std::vector<std::string> cardKeys = {
        "DNU-Birthday", "DNU-DoctorFee", "DNU-RunForOffice",
        "KKS-Demolition", "KKS-Discount", "KKS-Lasso",
        "KKS-Move", "KKS-Shield", "KKS-Teleport",
        "KSP-GoToJail", "KSP-MoveBack", "KSP-NearStation"};
    std::vector<std::string> codes;
    for (auto &key : cardKeys)
    {
        std::string path = "assets/card/" + key + ".png";
        if (FileExists(path.c_str()))
            cardTextures[key] = LoadTexture(path.c_str());
    }
    for (auto &t : tiles)
    {
        bool found = false;
        for (auto &c : codes)
            if (c == t.code)
            {
                found = true;
                break;
            }
        if (!found)
            codes.push_back(t.code);
    }
    for (auto &code : codes)
    {
        std::string path = "assets/tiles/" + code + ".png";
        if (FileExists(path.c_str()))
            tileTextures[code] = LoadTexture(path.c_str());
    }
    if (FileExists("assets/middle_deck/KSP-SourceCard.png"))
        deckKSP = LoadTexture("assets/middle_deck/KSP-SourceCard.png");
    if (FileExists("assets/middle_deck/DNU-SourceCard.png"))
        deckDNU = LoadTexture("assets/middle_deck/DNU-SourceCard.png");
}

// ─── Mock state ───────────────────────────────────────────────────────────────
void GameScreen::initMockState()
{
    gameState.currentTurn = 15;
    gameState.maxTurn = 50;
    gameState.activePlayerIdx = 0;

    gameState.players = {
        {"Uname1", 1500, 0, "ACTIVE", 2, true},
        {"Uname2", 800, 5, "ACTIVE", 1, false},
        {"Uname3", 2200, 11, "ACTIVE", 3, false},
        {"Uname4", 0, 10, "JAILED", 0, false},
    };

    gameState.properties.resize(40);
    for (int i = 0; i < 40; i++)
    {
        gameState.properties[i].code = TILE_DEFS[i].code;
        gameState.properties[i].owner = -1;
        gameState.properties[i].buildings = 0;
        gameState.properties[i].mortgaged = false;
        gameState.properties[i].festivalMult = 1;
        gameState.properties[i].festivalDur = 0;
        gameState.properties[i].type = "ACTION";
        gameState.properties[i].colorGroup = "";
    }

    auto setProp = [&](int idx, const std::string &name, const std::string &grp,
                       int owner, int bld, bool mort, int fmult, int fdur,
                       int p, int mv, int hu, int hotu,
                       int r0, int r1, int r2, int r3, int r4, int r5)
    {
        auto &pr = gameState.properties[idx];
        pr.name = name;
        pr.colorGroup = grp;
        pr.type = "STREET";
        pr.owner = owner;
        pr.buildings = bld;
        pr.mortgaged = mort;
        pr.festivalMult = fmult;
        pr.festivalDur = fdur;
        pr.price = p;
        pr.mortgageVal = mv;
        pr.houseUpg = hu;
        pr.hotelUpg = hotu;
        pr.rentL0 = r0;
        pr.rentL1 = r1;
        pr.rentL2 = r2;
        pr.rentL3 = r3;
        pr.rentL4 = r4;
        pr.rentL5 = r5;
    };

    setProp(31, "Bandung", "HIJAU", 1, 1, false, 2, 2, 300, 250, 200, 200, 26, 130, 390, 900, 1100, 1275);
    setProp(37, "Jakarta", "BIRU_TUA", 0, 0, false, 1, 0, 350, 300, 200, 200, 35, 175, 500, 1100, 1300, 1500);
    setProp(1, "Garut", "COKLAT", 0, 0, true, 1, 0, 60, 40, 20, 50, 2, 10, 30, 90, 160, 250);
    setProp(21, "Makassar", "MERAH", 2, 2, false, 4, 1, 220, 175, 150, 150, 18, 90, 250, 700, 875, 1050);
    setProp(9, "Bekasi", "BIRU_MUDA", -1, 0, false, 1, 0, 120, 60, 50, 50, 8, 40, 100, 300, 450, 600);
}

// ─── Input ────────────────────────────────────────────────────────────────────
void GameScreen::handleInput()
{
    if (IsKeyPressed(KEY_W))
        gameOver = true;

    if (IsKeyPressed(KEY_T) && isRealMode())
    {
        GameMaster *gm = guiManager->getGameMaster();
        Player *cur = gm->getState().getCurrPlayer();
        if (cur && selectedTile >= 0)
        {
            gm->teleportPlayer(cur, selectedTile, true);
            std::cout << "[DEBUG] Teleport ke tile " << selectedTile
                      << " (" << TILE_DEFS[selectedTile].code << ")" << std::endl;
        }
    }

    Vector2 mouse = GetMousePosition();

    float cx1 = boardX + CORNER_SZ;
    float cx2 = boardX + CORNER_SZ + 9 * TILE_W;
    float cy1 = boardY + CORNER_SZ;
    float cy2 = boardY + CORNER_SZ + 9 * TILE_W;

    // Zoom (scroll wheel, center area only)
    if (mouse.x > cx1 && mouse.x < cx2 && mouse.y > cy1 && mouse.y < cy2)
    {
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            float oldZoom = zoomLevel;
            zoomLevel = Clamp(zoomLevel + wheel * 0.1f, 0.5f, 3.f);
            zoomOffset.x += (mouse.x - cx1 - zoomOffset.x) * (1 - zoomLevel / oldZoom);
            zoomOffset.y += (mouse.y - cy1 - zoomOffset.y) * (1 - zoomLevel / oldZoom);
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            isDragging = true;
            dragStart = mouse;
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        isDragging = false;
    if (isDragging)
    {
        zoomOffset.x += mouse.x - dragStart.x;
        zoomOffset.y += mouse.y - dragStart.y;
        dragStart = mouse;
    }

    // Tile click → popup
    // Tile click → popup / Teleport target
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isDragging)
    {
        int hit = tileAtPoint(mouse);

        if (hit >= 0)
        {
            if (skillCardDialog.awaitingTeleportTile)
            {
                handleTeleportTargetClick(hit);
                showPopup = false;
                selectedTile = -1;
                return;
            }

            selectedTile = hit;
        }
        else
        {
            showPopup = false;
            selectedTile = -1;
        }
    }
}