#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "raymath.h"
#include "../../core/Commands/LemparDaduCommand.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Commands/BeliCommand.hpp"

#include "../../core/utils/SaveLoadManager.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include <cmath>
#include <string>
#include <algorithm>
#include <filesystem>

// ─── Tile order (clockwise, starting from GO = bottom-right corner) ──────────
static const TileDef TILE_DEFS[40] = {
    // BOTTOM ROW right→left (spec 1..11)
    {"GO", "BOTTOM", true},   // 0
    {"GRT", "BOTTOM", false}, // 1
    {"DNU", "BOTTOM", false}, // 2
    {"TSK", "BOTTOM", false}, // 3
    {"PPH", "BOTTOM", false}, // 4
    {"GBR", "BOTTOM", false}, // 5
    {"BGR", "BOTTOM", false}, // 6
    {"FES", "BOTTOM", false}, // 7
    {"DPK", "BOTTOM", false}, // 8
    {"BKS", "BOTTOM", false}, // 9
    {"PEN", "BOTTOM", true},  // 10 bottom-left corner

    // LEFT COLUMN bottom->top (spec 20..12)
    {"MGL", "LEFT", false}, // 11  ← paling bawah setelah PEN
    {"PLN", "LEFT", false}, // 12
    {"SOL", "LEFT", false}, // 13  (DNU di aksi = index 13, tapi code DNU)
    {"YOG", "LEFT", false}, // 14
    {"STB", "LEFT", false}, // 15  (railroad)
    {"MAL", "LEFT", false}, // 16
    {"SMG", "LEFT", false}, // 17
    {"DNU", "LEFT", false}, // 18  ← DNU dari aksi.txt
    {"SBY", "LEFT", false}, // 19  ← paling atas sebelum BBP

    // TOP ROW left→right (spec 21..31)
    {"BBP", "TOP", true},  // 20 top-left corner
    {"MKS", "TOP", false}, // 21
    {"KSP", "TOP", false}, // 22
    {"BLP", "TOP", false}, // 23
    {"MND", "TOP", false}, // 24
    {"TUG", "TOP", false}, // 25
    {"PLB", "TOP", false}, // 26
    {"PKB", "TOP", false}, // 27
    {"PAM", "TOP", false}, // 28
    {"MED", "TOP", false}, // 29
    {"PPJ", "TOP", true},  // 30 top-right corner

    // RIGHT COLUMN top→bottom (spec 32..40)
    {"BDG", "RIGHT", false}, // 31
    {"DEN", "RIGHT", false}, // 32
    {"FES", "RIGHT", false}, // 33
    {"MTR", "RIGHT", false}, // 34
    {"GUB", "RIGHT", false}, // 35
    {"KSP", "RIGHT", false}, // 36
    {"JKT", "RIGHT", false}, // 37
    {"PBM", "RIGHT", false}, // 38
    {"IKN", "RIGHT", false}, // 39
};

// Helper
static void DrawRoundedBorder(Rectangle rec, float roundness, int segments, float thick, Color color)
{
    for (float i = 0.f; i < thick; i += 0.5f)
    {
        Rectangle r = {rec.x - i, rec.y - i, rec.width + i * 2, rec.height + i * 2};
        DrawRectangleRoundedLines(r, roundness, segments, thick, color);
    }
}

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
    for (int i = 0; i < 40; i++)
        tiles.push_back(TILE_DEFS[i]);
    TraceLog(LOG_INFO, "Working dir: %s", GetWorkingDirectory());
    loadTextures();
    initMockState();
    initMockLogs(); // Load Logs
}

void GameScreen::onExit()
{
    for (auto &[k, v] : tileTextures)
        UnloadTexture(v);
    if (deckKSP.id != 0)
        UnloadTexture(deckKSP);
    if (deckDNU.id != 0)
        UnloadTexture(deckDNU);
    tiles.clear();
    tileTextures.clear();
}

bool GameScreen::isRealMode() const
{
    return guiManager != nullptr && guiManager->getGameMaster() != nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
//  syncFromGameMaster()
//
//  Dipanggil setiap frame di update() jika isRealMode() == true.
//  Fungsi ini mengisi ulang MockGameState dari data real sehingga semua
//  fungsi render (drawBoard, drawLeftPanel, dll.) tidak perlu diubah.
//
//  Apa yang disync:
//    - currentTurn, maxTurn
//    - players (username, balance, position, status, cardCount, isCurrentTurn)
//    - properties (owner, mortgaged, status — dari Board + Property*)
//
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::syncFromGameMaster()
{
    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    const GameState &gs = gm->getState();
    Board *board = gs.getBoard();

    // ── Turn info ────────────────────────────────────────────────────────
    gameState.currentTurn = gs.getCurrTurn();
    gameState.maxTurn = gs.getMaxTurn();
    gameState.activePlayerIdx = gs.getCurrPlayerIdx();

    // ── Sync players ─────────────────────────────────────────────────────
    const auto &realPlayers = gs.getPlayers();
    gameState.players.resize(realPlayers.size());

    for (int i = 0; i < (int)realPlayers.size(); ++i)
    {
        Player *p = realPlayers[i];
        MockPlayer &mp = gameState.players[i];

        mp.username = p->getUsername();
        mp.money = p->getBalance();
        mp.position = std::max(0, p->getPosition() - 1);
        mp.cardCount = p->getHandSize();
        mp.isCurrentTurn = (i == gs.getCurrPlayerIdx());

        switch (p->getStatus())
        {
        case PlayerStatus::ACTIVE:
            mp.status = "ACTIVE";
            break;
        case PlayerStatus::JAILED:
            mp.status = "JAILED";
            break;
        case PlayerStatus::BANKRUPT:
            mp.status = "BANKRUPT";
            break;
        }
    }

    // ── Sync properties dari Board ───────────────────────────────────────
    //
    // TILE_DEFS adalah array 40 elemen yang mendefinisikan urutan petak.
    // Kita iterasi semua tile, ambil PropertyTile* jika ada, lalu sync
    // MockProperty yang sesuai.
    //
    // Asumsi: gameState.properties sudah diinisialisasi dengan 40 slot
    // oleh initMockState(). Kita hanya update field owner/mortgaged/status.

    if (board)
    {
        for (int i = 0; i < board->getSize(); ++i)
        {
            Tile *tile = board->getTile(i);
            if (!tile)
                continue;

            // Cast ke PropertyTile — nullptr jika bukan properti
            PropertyTile *pt = dynamic_cast<PropertyTile *>(tile);

            if (!pt)
                continue;

            Property *prop = pt->getProperty();
            if (!prop)
                continue;

            // Pastikan indeks aman
            if (i >= (int)gameState.properties.size())
                continue;

            MockProperty &mp = gameState.properties[i];

            // Sync status kepemilikan
            switch (prop->getStatus())
            {
            case PropertyStatus::BANK:
                mp.owner = -1;
                mp.mortgaged = false;
                break;
            case PropertyStatus::OWNED:
            {
                // Cari indeks pemain berdasarkan ownerId
                mp.mortgaged = false;
                mp.owner = -1;
                const std::string &ownerId = prop->getOwnerId();
                for (int pi = 0; pi < (int)realPlayers.size(); ++pi)
                {
                    if (realPlayers[pi]->getUsername() == ownerId)
                    {
                        mp.owner = pi;
                        break;
                    }
                }
                break;
            }
            case PropertyStatus::MORTGAGED:
                mp.mortgaged = true;
                // Owner tetap ada meski digadai — cari seperti OWNED
                {
                    const std::string &ownerId = prop->getOwnerId();
                    mp.owner = -1;
                    for (int pi = 0; pi < (int)realPlayers.size(); ++pi)
                    {
                        if (realPlayers[pi]->getUsername() == ownerId)
                        {
                            mp.owner = pi;
                            break;
                        }
                    }
                }
                break;
            }
            if (dynamic_cast<StreetTile *>(pt))
                mp.type = "STREET";
            else if (dynamic_cast<RailRoadTile *>(pt))
                mp.type = "RAILROAD";
            else if (dynamic_cast<UtilityTile *>(pt))
                mp.type = "UTILITY";

            // Harga beli (untuk buy dialog)
            mp.price = prop->getPurchasePrice();
        }
    }

    // ── Sync ke UI (tombol, phase) ───────────────────────────────────────

    // ── Sync logger ──────────────────────────────────────────────────────
    // Logger real ada di GameState. Kita tidak copy isinya (mahal) — sebagai
    // gantinya, drawLogPopup() harus membaca dari sumber yang benar.
    // Lihat catatan di bawah tentang patch drawLogPopup().
}

// ─────────────────────────────────────────────────────────────────────────────
//  syncDiceResult()
//
//  Dipanggil dari GUIManager::run() SETELAH flushCommands(), karena
//  LemparDaduCommand baru mengubah nilai Dice di dalam execute().
//
//  Mengisi diceState dari Dice* real sehingga drawDiceArea() langsung
//  menampilkan hasil yang benar tanpa perlu ubah render code.
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::syncDiceResult()
{
    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    Dice *dice = gm->getState().getDice();
    if (!dice)
        return;

    int v1 = dice->getDaduVal1();
    int v2 = dice->getDaduVal2();

    // Jika nilai dadu berubah dari yang terakhir dirender → update & animasikan
    bool changed = (v1 != diceState.val1 || v2 != diceState.val2);

    if (changed && v1 > 0 && v2 > 0)
    {
        diceState.val1 = v1;
        diceState.val2 = v2;
        diceState.isDouble = dice->isDouble();
        diceState.tripleDouble = (dice->getConsecutiveDoubles() >= 3);
        diceState.hasRolled = gm->getState().getHasRolled();
        diceState.animating = true;
        diceState.animTimer = 0.f;
    }

    // Sync hasRolled jika berubah tanpa nilai dadu berubah
    // (misal setelah giliran berganti → hasRolled direset ke false)
    diceState.hasRolled = gm->getState().getHasRolled();

    // ── Cek apakah perlu munculkan buy dialog ────────────────────────────
    // Buy dialog dipicu saat phase = AWAITING_BUY dan dialog belum tampil
    const GameState &gs = gm->getState();
    if (gs.getPhase() == GamePhase::AWAITING_BUY && !buyDialog.visible)
    {
        Player *curP = gs.getCurrPlayer();
        if (curP)
        {
            int pos = curP->getPosition();
            // Cek properti di posisi ini masih milik bank (status BANK)
            Board *board = gs.getBoard();
            if (board)
            {
                Tile *tile = board->getTile(pos);
                PropertyTile *pt = dynamic_cast<PropertyTile *>(tile);
                if (pt && pt->getProperty())
                {
                    Property *prop = pt->getProperty();
                    bool canAfford = curP->canAfford(prop->getPurchasePrice());
                    // Hanya trigger untuk StreetTile (Railroad & Utility otomatis)
                    if (prop->getStatus() == PropertyStatus::BANK)
                    {
                        std::string tileType = gameState.properties[pos].type;
                        if (tileType == "STREET")
                        {
                            triggerBuyDialog(pos);
                            buyDialog.canAfford = canAfford;
                        }
                    }
                }
            }
        }
    }

    // Sembunyikan buy dialog jika phase sudah berubah keluar dari AWAITING_BUY
    if (buyDialog.visible && gs.getPhase() != GamePhase::AWAITING_BUY)
    {
        buyDialog.visible = false;
    }
}

// ─── Update ──────────────────────────────────────────────────────────────────
void GameScreen::update(float dt)
{
    // ── Sync data dari GameMaster (jika mode real) ───────────────────────
    //    syncDiceResult() dipanggil dari GUIManager::run() setelah flushCommands()
    //    sehingga posisinya sudah tepat (setelah Command dieksekusi).
    syncFromGameMaster();

    if (savePopup.resultTimer > 0)
        savePopup.resultTimer -= dt;

    // ── Animasi dadu ─────────────────────────────────────────────────────
    if (diceState.animating)
    {
        diceState.animTimer += dt;
        if (diceState.animTimer >= DiceState::ANIM_DURATION)
        {
            diceState.animating = false;
            diceState.animTimer = 0.f;
        }
    }

    // ── Log popup scroll ─────────────────────────────────────────────────
    if (showLogPopup)
    {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.f)
            logScrollY -= wheel * 24.f;
        if (logScrollY < 0.f)
            logScrollY = 0.f;
    }

    // ── Input keyboard & mouse ───────────────────────────────────────────
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
    drawDiceArea(); // overlay dadu di tengah board
    drawPopup();
    drawBuyDialog();
    drawLogPopup();
    drawSavePopup();
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
    std::vector<std::string> codes;
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
    // gameState.kspGlow         = false;
    // gameState.dnuGlow         = true;

    gameState.players = {
        {"Uname1", 1500, 0, "ACTIVE", 2, true},   // GO
        {"Uname2", 800, 5, "ACTIVE", 1, false},   // GBR
        {"Uname3", 2200, 11, "ACTIVE", 3, false}, // SBY
        {"Uname4", 0, 10, "JAILED", 0, false},    // PEN
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
    // Testing — tekan W untuk simulasi game over
    if (IsKeyPressed(KEY_W))
        gameOver = true;

    Vector2 mouse = GetMousePosition();

    float cx1 = boardX + CORNER_SZ;
    float cx2 = boardX + CORNER_SZ + 9 * TILE_W;
    float cy1 = boardY + CORNER_SZ;
    float cy2 = boardY + CORNER_SZ + 9 * TILE_W;

    // Zoom (scroll wheel, only inside center area)
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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isDragging)
    {
        int hit = tileAtPoint(mouse);
        if (hit >= 0)
        {
            selectedTile = hit;
            showPopup = true;
        }
        else
        {
            showPopup = false;
            selectedTile = -1;
        }
    }
}

// ─── Tile geometry ────────────────────────────────────────────────────────────
Vector2 GameScreen::getTileCenter(int idx)
{
    auto &td = TILE_DEFS[idx];
    float x = 0, y = 0;
    float boardSz = CORNER_SZ + 9 * TILE_W + CORNER_SZ;

    if (td.side == "BOTTOM")
    {
        if (td.corner)
        {
            // GO = kanan bawah, PEN = kiri bawah
            x = (idx == 0) ? boardX + boardSz - CORNER_SZ / 2.f
                           : boardX + CORNER_SZ / 2.f;
            y = boardY + boardSz - CORNER_SZ / 2.f;
        }
        else
        {
            // slot 0 = paling kiri (BKS), slot 8 = paling kanan (GRT)
            // idx 1=GRT, idx 9=BKS → slot = 9 - idx (dari kanan ke kiri)
            int slot = 9 - idx; // GRT(1)→slot8, BKS(9)→slot0
            x = boardX + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
            y = boardY + boardSz - TILE_H / 2.f;
        }
    }
    else if (td.side == "LEFT")
    {
        int slot = 8 - (idx - 11); // MGL(11)→slot8(bawah), SBY(19)→slot0(atas)
        x = boardX + TILE_H / 2.f;
        y = boardY + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
    }
    else if (td.side == "TOP")
    {
        if (td.corner)
        {
            x = (idx == 20) ? boardX + CORNER_SZ / 2.f
                            : boardX + boardSz - CORNER_SZ / 2.f;
            y = boardY + CORNER_SZ / 2.f;
        }
        else
        {
            int slot = idx - 21; // MKS(21)=0, MED(29)=8
            x = boardX + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
            y = boardY + TILE_H / 2.f;
        }
    }
    else
    {                        // RIGHT
        int slot = idx - 31; // BDG(31)=0, IKN(39)=8
        x = boardX + boardSz - TILE_H / 2.f;
        y = boardY + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
    }
    return {x, y};
}

Rectangle GameScreen::getTileRect(int idx)
{
    Vector2 c = getTileCenter(idx);
    auto &td = TILE_DEFS[idx];
    float hw, hh;
    if (td.corner)
    {
        hw = CORNER_SZ / 2.f;
        hh = CORNER_SZ / 2.f;
    }
    else if (td.side == "BOTTOM" || td.side == "TOP")
    {
        hw = TILE_H / 2.f;
        hh = TILE_W / 2.f;
    }
    else
    {
        hw = TILE_W / 2.f;
        hh = TILE_H / 2.f;
    }
    return {c.x - hw, c.y - hh, hw * 2, hh * 2};
}

int GameScreen::tileAtPoint(Vector2 pt)
{
    for (int i = 0; i < 40; i++)
        if (CheckCollisionPointRec(pt, getTileRect(i)))
            return i;
    return -1;
}

// ─── Draw tile ───────────────────────────────────────────────────────────────
void GameScreen::drawTile(int idx, float cx, float cy, float rotation)
{
    auto &td = TILE_DEFS[idx];
    auto &prop = gameState.properties[idx];
    float tw = td.corner ? CORNER_SZ : TILE_W;
    float th = td.corner ? CORNER_SZ : TILE_H;

    auto it = tileTextures.find(td.code);
    if (it != tileTextures.end())
    {
        Texture2D &tex = it->second;
        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dst = {cx, cy, tw, th};
        DrawTexturePro(tex, src, dst, {tw / 2.f, th / 2.f}, rotation, WHITE);
    }
    else
    {
        Rectangle r = {cx - tw / 2.f, cy - th / 2.f, tw, th};
        DrawRectangleRec(r, DARKGRAY);
        DrawRectangleLinesEx(r, 1, BLACK);
        int fw = MeasureText(td.code.c_str(), 9);
        DrawText(td.code.c_str(), (int)(cx - fw / 2), (int)(cy - 5), 9, WHITE);
    }

    // Festival ring
    if (prop.festivalMult > 1)
    {
        float pulse = 0.7f + 0.3f * sinf(glowTimer * 4.f);
        float thick = 1.5f * log2f((float)prop.festivalMult);
        Color rc;
        if (prop.festivalMult == 2)
            rc = {239, 159, 39, (unsigned char)(200 * pulse)};
        else if (prop.festivalMult == 4)
            rc = {186, 117, 23, (unsigned char)(220 * pulse)};
        else
            rc = {133, 79, 11, (unsigned char)(240 * pulse)};
        DrawRectangleLinesEx({cx - tw / 2.f - thick, cy - th / 2.f - thick, tw + 2 * thick, th + 2 * thick}, thick, rc);
        std::string lbl = "x" + std::to_string(prop.festivalMult);
        DrawText(lbl.c_str(), (int)(cx - tw / 2.f + 2), (int)(cy - th / 2.f + 2), 10, rc);
    }

    // Owner strip + buildings
    if (prop.owner >= 0 && prop.type == "STREET")
    {
        drawBuildingStrip(cx, cy, rotation, prop.buildings, playerColors[prop.owner]);
        DrawRectangleLinesEx({cx - tw / 2.f, cy - th / 2.f, tw, th}, 5, playerColors[prop.owner]);
        if (prop.mortgaged)
        {
            DrawRectangleRec({cx - tw / 2.f, cy - th / 2.f, tw, th}, {0, 0, 0, 140});
            int fw = MeasureText("GADAI", 10);
            DrawText("GADAI", (int)(cx - fw / 2), (int)(cy - 5), 10, RED);
        }
    }
    else if (prop.owner >= 0)
    {
        DrawRectangleLinesEx({cx - tw / 2.f, cy - th / 2.f, tw, th}, 3, playerColors[prop.owner]);
    }
}

// ─── Building strip ───────────────────────────────────────────────────────────
void GameScreen::drawBuildingStrip(float cx, float cy, float rotation,
                                   int buildings, Color ownerColor)
{
    float sw = STRIP_W, sh = STRIP_H;
    float localY = TILE_H / 2.f - sh / 2.f;
    float rad = rotation * DEG2RAD;
    float cosR = cosf(rad), sinR = sinf(rad);
    float sx = cx - localY * sinR;
    float sy = cy + localY * cosR;

    DrawRectanglePro({sx, sy, sw, sh}, {sw / 2.f, sh / 2.f}, rotation, ownerColor);

    if (buildings == 0)
        return;

    float bldLocalY = localY - sh;
    bool isHotel = (buildings == 5);

    if (isHotel)
    {
        float hw = sw * 0.85f, hh = sh * 0.7f;
        float bsx = cx - bldLocalY * sinR;
        float bsy = cy + bldLocalY * cosR;
        DrawRectanglePro({bsx, bsy, hw, hh}, {hw / 2.f, hh / 2.f}, rotation, ownerColor);
        DrawRectanglePro({bsx, bsy, hw, hh}, {hw / 2.f, hh / 2.f}, rotation, {0, 0, 0, 80});
    }
    else
    {
        float sqSz = sh * 0.75f;
        float totalW = sw * 0.9f;
        float gap = (buildings > 1) ? (totalW - buildings * sqSz) / (buildings - 1) : 0;
        float startX = -(totalW / 2.f) + sqSz / 2.f;
        for (int b = 0; b < buildings; b++)
        {
            float lx = startX + b * (sqSz + gap);
            float bsx = cx + lx * cosR - bldLocalY * sinR;
            float bsy = cy + lx * sinR + bldLocalY * cosR;
            DrawRectanglePro({bsx, bsy, sqSz, sqSz}, {sqSz / 2.f, sqSz / 2.f}, rotation, ownerColor);
            DrawRectanglePro({bsx, bsy, sqSz, sqSz}, {sqSz / 2.f, sqSz / 2.f}, rotation, {0, 0, 0, 60});
        }
    }
}

// ─── Draw players on tile ─────────────────────────────────────────────────────
void GameScreen::drawPlayers(int tileIdx, float cx, float cy)
{
    std::vector<int> onTile;
    for (int p = 0; p < (int)gameState.players.size(); p++)
        if (gameState.players[p].position == tileIdx)
            onTile.push_back(p);
    if (onTile.empty())
        return;

    float r = 8.f, sp = r * 2.2f;
    float startX = cx - (onTile.size() * sp) / 2.f + r;
    for (int i = 0; i < (int)onTile.size(); i++)
    {
        int pi = onTile[i];
        float px = startX + i * sp;
        DrawCircle((int)px, (int)cy, r + 3, WHITE);
        DrawCircle((int)px, (int)cy, r, playerColors[pi]);
        std::string num = std::to_string(pi + 1);
        int fw = MeasureText(num.c_str(), 9);
        DrawText(num.c_str(), (int)(px - fw / 2.f), (int)(cy - 5), 9, WHITE);
    }
}

// ─── Draw board ───────────────────────────────────────────────────────────────
void GameScreen::drawBoard()
{
    float sz = CORNER_SZ + 9 * TILE_W + CORNER_SZ;
    DrawRectangle((int)boardX, (int)boardY, (int)sz, (int)sz, {230, 225, 210, 255});
    DrawRectangleLinesEx({boardX, boardY, sz, sz}, 2, {180, 170, 150, 255});

    for (int i = 0; i < 40; i++)
    {
        Vector2 c = getTileCenter(i);
        float rot = 0;
        std::string side = TILE_DEFS[i].side;
        if (side == "BOTTOM")
            rot = 0;
        else if (side == "LEFT")
            rot = 270;
        else if (side == "TOP")
            rot = 180;
        else
            rot = 90;
        drawTile(i, c.x, c.y, rot);
        drawPlayers(i, c.x, c.y);
    }
    drawCenterArea();
}

void GameScreen::drawCenterArea()
{
    float cx1 = boardX + CORNER_SZ;
    float cy1 = boardY + CORNER_SZ;
    float cw = 9 * TILE_W;
    float ch = 9 * TILE_W;
    float ccx = cx1 + cw / 2.f;
    float ccy = cy1 + ch / 2.f;

    BeginScissorMode((int)cx1, (int)cy1, (int)cw, (int)ch);

    float ox = zoomOffset.x, oy = zoomOffset.y;

    // ── Title (tengah, bebas untuk kamu isi sendiri nanti) ────────────────
    DrawText("NIMONSPOLI",
             (int)(ccx + ox - 60), (int)(ccy + oy - 80), (int)(36 * zoomLevel), {80, 60, 40, 200});
    std::string ts = "Turn " + std::to_string(gameState.currentTurn) +
                     " / " + std::to_string(gameState.maxTurn);
    DrawText(ts.c_str(), (int)(ccx + ox - 50), (int)(ccy + oy - 30), (int)(18 * zoomLevel), {100, 80, 60, 200});

    // ── Card sizing (auto aspect ratio dari texture asli) ─────────────────
    float cardW = 220 * zoomLevel;
    float cardH = (deckKSP.id != 0)
                      ? cardW * ((float)deckKSP.height / deckKSP.width)
                      : cardW * 1.4f;

    float margin = 100 * zoomLevel;

    // KSP — pojok kiri bawah, miring 135
    float kspCX = cx1 + ox + margin + cardW / 2.f;
    float kspCY = cy1 + oy + margin + 30 + cardH / 2.f;
    float kspRot = 135.f;

    // DNU — pojok kanan bawah, miring -45
    float dnuCX = cx1 + ox + cw - margin - cardW / 2.f;
    float dnuCY = cy1 + oy + ch - margin - 30 - cardH / 2.f;
    float dnuRot = -45.f;

    // ── KSP glow ──────────────────────────────────────────────────────────
    if (kspGlowing)
    {
        auto a = (unsigned char)(160 + 80 * sinf(glowTimer * 5.f));
        // glow sebagai rectangle di belakang card (ikut posisi card)
        DrawRectanglePro({kspCX, kspCY, cardW + 20, cardH + 20},
                         {(cardW + 20) / 2.f, (cardH + 20) / 2.f},
                         kspRot, {255, 220, 0, a});
    }

    // ── DNU glow ──────────────────────────────────────────────────────────
    if (dnuGlowing)
    {
        auto a = (unsigned char)(160 + 80 * sinf(glowTimer * 5.f));
        DrawRectanglePro({dnuCX, dnuCY, cardW + 20, cardH + 20},
                         {(cardW + 20) / 2.f, (cardH + 20) / 2.f},
                         dnuRot, {220, 50, 50, a});
    }

    // ── KSP card ──────────────────────────────────────────────────────────
    if (deckKSP.id != 0)
        DrawTexturePro(deckKSP,
                       {0, 0, (float)deckKSP.width, (float)deckKSP.height},
                       {kspCX, kspCY, cardW, cardH},
                       {cardW / 2.f, cardH / 2.f},
                       kspRot, WHITE);
    else
    {
        DrawRectanglePro({kspCX, kspCY, cardW, cardH}, {cardW / 2.f, cardH / 2.f}, kspRot, {255, 180, 0, 200});
        DrawText("KSP", (int)(kspCX - 10), (int)(kspCY - 8), 14, WHITE);
    }

    // ── DNU card ──────────────────────────────────────────────────────────
    if (deckDNU.id != 0)
        DrawTexturePro(deckDNU,
                       {0, 0, (float)deckDNU.width, (float)deckDNU.height},
                       {dnuCX, dnuCY, cardW, cardH},
                       {cardW / 2.f, cardH / 2.f},
                       dnuRot, WHITE);
    else
    {
        DrawRectanglePro({dnuCX, dnuCY, cardW, cardH}, {cardW / 2.f, cardH / 2.f}, dnuRot, {180, 50, 50, 200});
        DrawText("DNU", (int)(dnuCX - 10), (int)(dnuCY - 8), 14, WHITE);
    }

    // ── Label "Klik untuk ambil!" ─────────────────────────────────────────
    if (kspGlowing)
        DrawText("Klik untuk ambil!",
                 (int)(kspCX - cardW / 2.f),
                 (int)(kspCY + cardH / 2.f + 8),
                 (int)(11 * zoomLevel), {200, 160, 0, 255});
    if (dnuGlowing)
        DrawText("Klik untuk ambil!",
                 (int)(dnuCX - cardW / 2.f),
                 (int)(dnuCY + cardH / 2.f + 8),
                 (int)(11 * zoomLevel), {200, 60, 60, 255});

    EndScissorMode();
}

// ─── Left panel ───────────────────────────────────────────────────────────────
void GameScreen::drawLeftPanel()
{
    DrawRectangle(0, 0, LEFT_PANEL, SCREEN_H, {30, 30, 40, 255});
    DrawRectangleLinesEx({0, 0, (float)LEFT_PANEL, (float)SCREEN_H}, 1, {60, 60, 80, 255});
    DrawText("PEMAIN", 12, 14, 14, {150, 150, 180, 255});
    DrawLine(8, 32, LEFT_PANEL - 8, 32, {60, 60, 80, 255});

    float panelH = (SCREEN_H - 50.f) / 4.f;

    for (int p = 0; p < (int)gameState.players.size(); p++)
    {
        auto &pl = gameState.players[p];
        Color col = playerColors[p];
        float py = 40 + p * panelH;
        bool active = (p == gameState.activePlayerIdx);

        Rectangle card = {8, py, LEFT_PANEL - 16, panelH - 6};
        DrawRectangleRec(card, active ? Color{45, 50, 65, 255} : Color{35, 37, 48, 255});
        DrawRectangleLinesEx(card, active ? 2.f : 1.f, active ? col : Color{60, 60, 80, 255});
        DrawRectangle(8, (int)py, 4, (int)(panelH - 6), col);

        // Avatar
        DrawCircle(22, (int)(py + 12), 10, col);
        std::string pn = std::to_string(p + 1);
        DrawText(pn.c_str(), 18, (int)(py + 6), 11, WHITE);

        DrawText(pl.username.c_str(), 38, (int)(py + 6), 13, WHITE);
        if (active)
            DrawText("▶ GILIRAN", LEFT_PANEL - 90, (int)(py + 6), 10, col);

        DrawText(("M " + std::to_string(pl.money)).c_str(), 38, (int)(py + 22), 12, {100, 220, 100, 255});
        DrawText(("@ " + std::string(TILE_DEFS[pl.position].code)).c_str(),
                 38, (int)(py + 36), 11, {180, 180, 180, 255});

        Color sc = {100, 220, 100, 255};
        if (pl.status == "JAILED")
            sc = {220, 100, 100, 255};
        if (pl.status == "BANKRUPT")
            sc = {120, 120, 120, 255};
        DrawText(pl.status.c_str(), 38, (int)(py + 50), 10, sc);

        for (int c = 0; c < pl.cardCount && c < 3; c++)
        {
            DrawRectangle(38 + c * 16, (int)(py + 64), 12, 16, {80, 100, 160, 255});
            DrawRectangleLinesEx({38.f + c * 16, py + 64, 12, 16}, 1, {120, 140, 200, 255});
        }
        if (pl.cardCount > 0)
            DrawText(("x" + std::to_string(pl.cardCount)).c_str(),
                     38 + pl.cardCount * 16 + 2, (int)(py + 66), 10, {150, 150, 200, 255});

        if (p >= activePlayerCount)
        {
            DrawRectangleRec(card, {0, 0, 0, 180});
            DrawText("+", (int)(card.x + card.width / 2 - 5),
                     (int)(card.y + card.height / 2 - 6), 14, {50, 52, 80, 255});
        }
    }
}

// ─── Right panel ──────────────────────────────────────────────────────────────
void GameScreen::drawRightPanel()
{
    float rx = SCREEN_W - RIGHT_PANEL;
    DrawRectangle((int)rx, 0, RIGHT_PANEL, SCREEN_H, {30, 30, 40, 255});
    DrawRectangleLinesEx({rx, 0, (float)RIGHT_PANEL, (float)SCREEN_H}, 1, {60, 60, 80, 255});

    DrawText("GILIRAN", (int)rx + 10, 14, 14, {150, 150, 180, 255});
    DrawLine((int)rx + 8, 32, SCREEN_W - 8, 32, {60, 60, 80, 255});

    std::string ts = std::to_string(gameState.currentTurn) + " / " +
                     std::to_string(gameState.maxTurn);
    DrawText("Turn", (int)rx + 10, 38, 11, {150, 150, 180, 255});
    DrawText(ts.c_str(), (int)rx + 10, 52, 16, WHITE);

    auto &curP = gameState.players[gameState.activePlayerIdx];
    DrawText("Giliran:", (int)rx + 10, 76, 11, {150, 150, 180, 255});
    DrawText(curP.username.c_str(), (int)rx + 10, 90, 14, playerColors[gameState.activePlayerIdx]);

    DrawLine((int)rx + 8, 112, SCREEN_W - 8, 112, {60, 60, 80, 255});
    DrawText("AKSI", (int)rx + 10, 120, 12, {150, 150, 180, 255});

    struct Btn
    {
        const char *label;
        Color col;
    };
    Btn btns[] = {
        {"LEMPAR DADU", {70, 130, 180, 255}},
        {"USE CARD", {100, 160, 100, 255}},
        {"BELI BANGUNAN", {80, 140, 80, 255}},
        {"GADAI", {180, 130, 60, 255}},
        {"TEBUS", {160, 120, 80, 255}},
        {"JUAL BANGUNAN", {160, 80, 80, 255}},
        {"SAVE", {100, 100, 160, 255}},
        {"END TURN", {80, 100, 160, 255}},
    };

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < 8; i++)
    {
        Rectangle btn = {rx + 10, 140.f + i * 44, RIGHT_PANEL - 20, 36};
        bool hover = CheckCollisionPointRec(mouse, btn);

        bool disabled = false;
        if (isRealMode())
        {
            // ── Mode real: kunci tombol berdasarkan GamePhase ─────────────
            GamePhase phase = guiManager->getGameMaster()->getState().getPhase();
            bool hasRolled = guiManager->getGameMaster()->getState().getHasRolled();
            switch (i)
            {
            case 0: // LEMPAR DADU
                disabled = hasRolled || diceState.animating || phase != GamePhase::PLAYER_TURN;
                break;
            case 1: // USE CARD
                disabled = phase != GamePhase::PLAYER_TURN || hasRolled;
                break;
            case 2: // BELI BANGUNAN
                disabled = phase != GamePhase::PLAYER_TURN;
                break;
            case 3: // GADAI
            case 4: // TEBUS
            case 5: // JUAL BANGUNAN
                disabled = (phase == GamePhase::AUCTION || phase == GamePhase::BANKRUPTCY || phase == GamePhase::GAME_OVER);
                break;
            case 6: // SAVE — hanya boleh di awal giliran (sebelum roll)
                disabled = hasRolled || phase != GamePhase::PLAYER_TURN;
                break;
            case 7:
                disabled = !hasRolled || phase != GamePhase::PLAYER_TURN;
                break;
            default:
                break;
            }
        }
        else
        {
            // ── Mode mock: logika lama ────────────────────────────────────
            if (i == 0)
                disabled = diceState.hasRolled || diceState.animating;
        }

        Color colBase = disabled ? Color{50, 50, 60, 255} : Color{40, 42, 54, 255};
        Color colHov = disabled ? Color{50, 50, 60, 255}
                                : Color{btns[i].col.r, btns[i].col.g, btns[i].col.b, 220};
        Color border = disabled ? Color{70, 70, 80, 255} : btns[i].col;

        DrawRectangleRec(btn, hover && !disabled ? colHov : colBase);
        DrawRectangleLinesEx(btn, 1, border);
        int tw = MeasureText(btns[i].label, 11);
        Color textCol = disabled ? Color{90, 90, 100, 255}
                                 : (hover ? WHITE : Color{200, 200, 210, 255});
        DrawText(btns[i].label, (int)(rx + RIGHT_PANEL / 2 - tw / 2),
                 (int)(140 + i * 44 + 12), 11, textCol);

        // ── Handle klik ──────────────────────────────────────────────────
        if (!disabled && hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            switch (i)
            {
            case 0:
                handleLemparDadu();
                break;
            // case 1: handleUseCard(); break;   // TODO
            // case 2: handleBangun(); break;     // TODO
            // case 3: handleGadai(); break;      // TODO
            // case 4: handleTebus(); break;      // TODO
            // case 5: handleJualBangunan(); break; // TODO
            case 6:
                handleSimpan();
                break; // TODO
            case 7:
                if (guiManager && guiManager->getGameMaster())
                {
                    GameMaster *gm = guiManager->getGameMaster();
                    gm->endTurn();
                    gm->beginTurn();
                }
                break;
            default:
                break;
            }
        }
    }

    const char *modeStr = isRealMode() ? "MODE: REAL" : "MODE: MOCK";
    Color modeCol = isRealMode() ? Color{100, 220, 100, 255} : Color{220, 100, 100, 255};
    DrawText(modeStr, (int)rx + 10, SCREEN_H - 20, 11, modeCol);

    DrawLine((int)rx + 8, SCREEN_H - 80, SCREEN_W - 8, SCREEN_H - 80, {60, 60, 80, 255});
    Rectangle logBtn = {rx + 10, (float)SCREEN_H - 70, RIGHT_PANEL - 20, 32};
    DrawRectangleRec(logBtn, {40, 42, 54, 255});
    DrawRectangleLinesEx(logBtn, 1, {100, 100, 160, 255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), logBtn))
        showLogPopup = !showLogPopup;
    int lw = MeasureText("LOG TRANSAKSI", 11);
    DrawText("LOG TRANSAKSI", (int)(rx + RIGHT_PANEL / 2 - lw / 2), SCREEN_H - 58, 11, {150, 150, 200, 255});
}

// ─── Property popup ───────────────────────────────────────────────────────────
void GameScreen::drawPopup()
{
    if (!showPopup || selectedTile < 0)
        return;
    auto &prop = gameState.properties[selectedTile];

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 140});

    float pw = 300, ph = 480;
    float px = SCREEN_W / 2.f - pw / 2.f, py = SCREEN_H / 2.f - ph / 2.f;

    DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {240, 238, 230, 255});
    DrawRectangleLinesEx({px, py, pw, ph}, 2, {80, 70, 60, 255});

    Color hdr = prop.colorGroup.empty() ? DARKGRAY : getGroupColor(prop.colorGroup);
    DrawRectangle((int)px, (int)py, (int)pw, 60, hdr);

    std::string name = prop.name.empty() ? TILE_DEFS[selectedTile].code : prop.name;
    int nw = MeasureText(name.c_str(), 18);
    DrawText(name.c_str(), (int)(px + pw / 2 - nw / 2), (int)(py + 8), 18, WHITE);
    int gw = MeasureText(prop.colorGroup.c_str(), 12);
    DrawText(prop.colorGroup.c_str(), (int)(px + pw / 2 - gw / 2), (int)(py + 30), 12, {255, 255, 255, 200});

    std::string status = "BANK";
    Color sc = GRAY;
    if (prop.owner >= 0)
    {
        status = gameState.players[prop.owner].username;
        sc = playerColors[prop.owner];
        if (prop.mortgaged)
        {
            status += " [GADAI]";
            sc = RED;
        }
    }
    DrawText(status.c_str(), (int)(px + 10), (int)(py + 68), 13, sc);

    float ry = py + 90;
    auto row = [&](const char *key, const std::string &val, Color vc = {40, 40, 40, 255})
    {
        DrawText(key, (int)(px + 10), (int)ry, 12, {120, 110, 100, 255});
        int vw = MeasureText(val.c_str(), 12);
        DrawText(val.c_str(), (int)(px + pw - 10 - vw), (int)ry, 12, vc);
        DrawLine((int)(px + 10), (int)(ry + 16), (int)(px + pw - 10), (int)(ry + 16), {200, 195, 185, 255});
        ry += 22;
    };

    if (prop.type == "STREET")
    {
        row("Harga beli", "M " + std::to_string(prop.price));
        row("Nilai gadai", "M " + std::to_string(prop.mortgageVal));
        ry += 6;
        DrawText("Tabel Sewa", (int)(px + 10), (int)ry, 11, {100, 90, 80, 255});
        ry += 18;
        row("Tanah kosong", "M " + std::to_string(prop.rentL0));
        row("Monopoli (×2)", "M " + std::to_string(prop.rentL0 * 2));
        row("1 rumah", "M " + std::to_string(prop.rentL1));
        row("2 rumah", "M " + std::to_string(prop.rentL2));
        row("3 rumah", "M " + std::to_string(prop.rentL3));
        row("4 rumah", "M " + std::to_string(prop.rentL4));
        row("Hotel ★", "M " + std::to_string(prop.rentL5));
        ry += 6;
        if (prop.festivalMult > 1)
        {
            row("Festival", "×" + std::to_string(prop.festivalMult) + " (" + std::to_string(prop.festivalDur) + " giliran)", {200, 140, 0, 255});
        }
        row("Upgrade rumah", "M " + std::to_string(prop.houseUpg));
        row("Upgrade hotel", "M " + std::to_string(prop.hotelUpg));
    }
    else
    {
        row("Jenis", prop.type);
    }

    // Buttons
    float btnY = py + ph - 50, btnW = pw / 3.f - 8;
    struct PBtn
    {
        const char *lbl;
        Color c;
    };
    PBtn pbts[] = {{"Tutup", {80, 80, 90, 255}}, {"Gadai", {180, 80, 60, 255}}, {"Bangun", {60, 140, 80, 255}}};
    for (int i = 0; i < 3; i++)
    {
        Rectangle b = {px + 6 + i * (btnW + 4), btnY, btnW, 36};
        DrawRectangleRec(b, pbts[i].c);
        DrawRectangleLinesEx(b, 1, {255, 255, 255, 60});
        int tw = MeasureText(pbts[i].lbl, 12);
        DrawText(pbts[i].lbl, (int)(b.x + btnW / 2 - tw / 2), (int)(btnY + 11), 12, WHITE);
        if (i == 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), b))
        {
            showPopup = false;
            selectedTile = -1;
        }
    }
}

// ─── Lempar Dadu ─────────────────────────────────────────────────────────────

void GameScreen::handleLemparDadu()
{
    // cout << "Debug: GAMEMODE " << (isRealMode() ? "REAL" : "MOCK") << endl;
    //  Jika ada GameMaster (mode real), push LemparDaduCommand ke GUIManager
    if (guiManager && guiManager->getGameMaster())
    {
        GameMaster *gm = guiManager->getGameMaster();
        Player *player = gm->getState().getCurrPlayer();
        Dice *dice = gm->getState().getDice();
        if (player && dice)
        {
            guiManager->pushCommand(new LemparDaduCommand(*gm, player, *dice));
        }
        return;
    }

    // ── Mode mock (GUI berjalan tanpa GameMaster) ─────────────────────────
    if (diceState.hasRolled || diceState.animating)
        return;

    // Simulasi roll dadu
    diceState.val1 = 4;
    diceState.val2 = 5;
    int total = diceState.val1 + diceState.val2;

    diceState.isDouble = (diceState.val1 == diceState.val2);
    diceState.tripleDouble = false; // mock tidak track consecutive doubles
    diceState.hasRolled = true;
    diceState.animating = true;
    diceState.animTimer = 0.f;

    // Update posisi mock player
    auto &curP = gameState.players[gameState.activePlayerIdx];
    int newPos = (curP.position + total) % 40;

    // Pindah pemain + log
    std::string detail = "Lempar: " + std::to_string(diceState.val1) + "+" +
                         std::to_string(diceState.val2) + "=" + std::to_string(total) +
                         ", mendarat di " + TILE_DEFS[newPos].code;
    if (diceState.isDouble)
        detail += " (double!)";

    gameState.logger.addLog(gameState.currentTurn, curP.username, "DADU", detail);
    curP.position = newPos;

    // ── Cek apakah mendarat di properti berstatus BANK (Street only) ──────
    // Railroad & Utility langsung berpindah tanpa dialog
    auto &landedProp = gameState.properties[newPos];

    if (landedProp.type == "STREET" && landedProp.owner == -1)
    {

        triggerBuyDialog(newPos);
    }
}

// ─── Handle Simpan ────────────────────────────────────────────────────────────
void GameScreen::handleSimpan()
{
    // Buka popup save — reset state
    savePopup.visible = true;
    savePopup.confirmOverwrite = false;
    savePopup.resultVisible = false;
    savePopup.fileNameInput = "save"; // default filename
}

// ─── Draw Save Popup ──────────────────────────────────────────────────────────
void GameScreen::drawSavePopup()
{
    if (!savePopup.visible)
        return;

    // Dim background
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 170});

    constexpr float PW = 480.f;
    constexpr float PH = 260.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    // Panel
    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 1.5f, {80, 80, 130, 255});

    // Header bar
    DrawRectangle((int)px, (int)py, (int)PW, 44, {32, 34, 52, 255});
    DrawRectangleLinesEx({px, py, PW, 44}, 1, {80, 80, 130, 255});
    DrawText("SIMPAN GAME", (int)(px + 16), (int)(py + 14), 15, {180, 180, 230, 255});

    // Tombol X
    Rectangle xBtn = {px + PW - 36, py + 8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);
    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
    {
        savePopup.visible = false;
        savePopup.confirmOverwrite = false;
    }

    if (!savePopup.confirmOverwrite)
    {
        // ── Mode normal: input nama file ──────────────────────────────────

        DrawText("Nama file:", (int)(px + 20), (int)(py + 62), 13, {150, 150, 200, 255});
        DrawText("(folder: data/)", (int)(px + 110), (int)(py + 64), 11, {80, 80, 120, 255});

        // Input box
        Rectangle inputBox = {px + 20, py + 84, PW - 100, 38};
        // bool inputHov = CheckCollisionPointRec(GetMousePosition(), inputBox);
        DrawRectangleRec(inputBox, {28, 30, 46, 255});
        DrawRectangleLinesEx(inputBox, 1.5f, {100, 100, 200, 255});

        // Ketik karakter
        if (IsKeyPressed(KEY_BACKSPACE) && !savePopup.fileNameInput.empty())
            savePopup.fileNameInput.pop_back();
        int ch = GetCharPressed();
        while (ch > 0)
        {
            bool valid = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                         (ch >= '0' && ch <= '9') || ch == '_' || ch == '-';
            if (valid && savePopup.fileNameInput.size() < 20)
                savePopup.fileNameInput += (char)ch;
            ch = GetCharPressed();
        }

        // Tampilkan teks + ekstensi
        std::string disp = savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput;
        DrawText((disp + ".txt").c_str(), (int)(inputBox.x + 12), (int)(inputBox.y + 11),
                 14, savePopup.fileNameInput.empty() ? Color{60, 60, 100, 255} : WHITE);

        // Cursor blink
        if ((int)(GetTime() * 2) % 2 == 0 && !savePopup.fileNameInput.empty())
        {
            int cw = MeasureText((savePopup.fileNameInput + ".txt").c_str(), 14);
            DrawText("|", (int)(inputBox.x + 14 + cw), (int)(inputBox.y + 11), 14,
                     {120, 120, 255, 255});
        }

        // Tombol SIMPAN
        Rectangle okBtn = {px + PW - 110, py + 84, 86, 38};
        bool okHov = CheckCollisionPointRec(GetMousePosition(), okBtn);
        DrawRectangleRec(okBtn, okHov ? Color{50, 100, 180, 255} : Color{35, 70, 130, 255});
        DrawRectangleLinesEx(okBtn, 1, {80, 130, 220, 255});
        int sw = MeasureText("SIMPAN", 12);
        DrawText("SIMPAN", (int)(okBtn.x + 43 - sw / 2), (int)(okBtn.y + 13), 12, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && okHov)
        {
            std::string fname = "data/" +
                                (savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput) + ".txt";
            bool exists = FileExists(fname.c_str());
            if (exists)
            {
                savePopup.confirmOverwrite = true;
            }
            else
            {
                // Langsung simpan
                doSave(fname);
            }
        }

        // Pesan hasil (setelah simpan)
        if (savePopup.resultVisible && savePopup.resultTimer > 0)
        {
            float alpha = std::min(1.f, savePopup.resultTimer);
            Color rc = savePopup.resultOk ? Color{80, 200, 120, (unsigned char)(255 * alpha)}
                                          : Color{220, 80, 80, (unsigned char)(255 * alpha)};
            int rw = MeasureText(savePopup.resultMsg.c_str(), 13);
            DrawText(savePopup.resultMsg.c_str(),
                     (int)(px + PW / 2 - rw / 2),
                     (int)(py + PH - 44), 13, rc);
        }

        // Petunjuk keyboard
        DrawText("Enter untuk konfirmasi · Esc untuk tutup",
                 (int)(px + 20), (int)(py + PH - 24), 11, {70, 70, 110, 255});

        // Enter shortcut
        if (IsKeyPressed(KEY_ENTER))
        {
            std::string fname = "data/" +
                                (savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput) + ".txt";
            if (FileExists(fname.c_str()))
                savePopup.confirmOverwrite = true;
            else
                doSave(fname);
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            savePopup.visible = false;
            savePopup.confirmOverwrite = false;
        }
    }
    else
    {
        // ── Mode konfirmasi timpa ─────────────────────────────────────────

        std::string fname = "data/" +
                            (savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput) + ".txt";

        std::string warn = "File \"" + fname + "\" sudah ada.";
        DrawText(warn.c_str(), (int)(px + 20), (int)(py + 70), 13, {220, 180, 60, 255});
        DrawText("Timpa file ini?", (int)(px + 20), (int)(py + 94), 14, WHITE);

        // YA
        Rectangle yaBtn = {px + 60, py + 136, 140, 44};
        bool yaHov = CheckCollisionPointRec(GetMousePosition(), yaBtn);
        DrawRectangleRec(yaBtn, yaHov ? Color{60, 180, 90, 255} : Color{40, 120, 60, 255});
        DrawRectangleLinesEx(yaBtn, 1, {80, 220, 120, 255});
        int yw = MeasureText("YA, TIMPA", 13);
        DrawText("YA, TIMPA", (int)(yaBtn.x + 70 - yw / 2), (int)(yaBtn.y + 15), 13, WHITE);

        // TIDAK
        Rectangle noBtn = {px + PW - 200, py + 136, 140, 44};
        bool noHov = CheckCollisionPointRec(GetMousePosition(), noBtn);
        DrawRectangleRec(noBtn, noHov ? Color{180, 60, 60, 255} : Color{120, 40, 40, 255});
        DrawRectangleLinesEx(noBtn, 1, {220, 80, 80, 255});
        int nw = MeasureText("TIDAK", 13);
        DrawText("TIDAK", (int)(noBtn.x + 70 - nw / 2), (int)(noBtn.y + 15), 13, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (yaHov)
            {
                doSave(fname);
                savePopup.confirmOverwrite = false;
            }
            else if (noHov)
            {
                savePopup.confirmOverwrite = false; // kembali ke input
            }
        }
        if (IsKeyPressed(KEY_ESCAPE))
            savePopup.confirmOverwrite = false;
    }
}

void GameScreen::doSave(const std::string &filepath)
{
    bool ok = false;

    if (isRealMode() && guiManager)
    {
        GameMaster *gm = guiManager->getGameMaster();
        if (gm)
        {
            try
            {
                // Buat folder data/ kalau belum ada
                std::filesystem::create_directories("data");

                SaveLoadManager slm;
                slm.save(gm->getState(), filepath);
                ok = true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "[SIMPAN] Exception: " << e.what() << std::endl;
                ok = false;
            }
            catch (...)
            {
                std::cerr << "[SIMPAN] Unknown exception saat save!" << std::endl;
                ok = false;
            }
        }
    }
    else
    {
        FILE *f = fopen(filepath.c_str(), "w");
        if (f)
        {
            fprintf(f, "MOCK_SAVE\n");
            fclose(f);
            ok = true;
        }
    }

    if (ok)
    {
        std::cout << "[SIMPAN] Berhasil disimpan ke: " << filepath << std::endl;
    }
    else
    {
        std::cerr << "[SIMPAN] Gagal menyimpan file: " << filepath << std::endl;
    }

    savePopup.resultOk = ok;
    savePopup.resultMsg = ok ? ("Tersimpan: " + filepath) : "Gagal menyimpan!";
    savePopup.resultVisible = true;
    savePopup.resultTimer = 3.f;

    if (ok)
        savePopup.visible = false;
}
// ─── Draw dice area ───────────────────────────────────────────────────────────

void GameScreen::drawDiceArea()
{
    // Area tengah papan (dalam inner board square)
    float boardSz = CORNER_SZ + 9 * TILE_W + CORNER_SZ;
    float innerX = boardX + CORNER_SZ;
    float innerY = boardY + CORNER_SZ;
    float innerSz = 9 * TILE_W;
    float cx = innerX + innerSz / 2.f;
    float cy = innerY + innerSz / 2.f;
    (void)boardSz;

    // Ukuran dadu
    constexpr float DIE_SZ = 54.f;
    constexpr float GAP = 16.f;
    float totalW = DIE_SZ * 2 + GAP;
    float d1x = cx - totalW / 2.f;
    float d2x = d1x + DIE_SZ + GAP;
    float dy = cy - DIE_SZ / 2.f;

    // ── Selama animasi: gambar angka acak berputar ────────────────────────
    int disp1 = diceState.val1;
    int disp2 = diceState.val2;
    if (diceState.animating)
    {
        float t = diceState.animTimer / DiceState::ANIM_DURATION;
        // Makin lambat menjelang akhir
        if (t < 0.85f)
        {
            disp1 = GetRandomValue(1, 6);
            disp2 = GetRandomValue(1, 6);
        }
    }

    // Belum pernah lempar — tampilkan placeholder "?"
    bool showResult = diceState.val1 > 0;

    // ── Helper lambda: gambar satu dadu ───────────────────────────────────
    auto drawDie = [&](float x, float y, int val, bool highlight)
    {
        Color bg = highlight ? Color{255, 230, 80, 255} : Color{240, 240, 240, 255};
        Color border = highlight ? Color{200, 160, 0, 255} : Color{160, 160, 160, 255};
        Color dotCol = {30, 30, 30, 255};

        // Sedikit goyang saat animasi
        if (diceState.animating)
        {
            float shake = 3.f * (1.f - diceState.animTimer / DiceState::ANIM_DURATION);
            x += (float)GetRandomValue(-1, 1) * shake;
            y += (float)GetRandomValue(-1, 1) * shake;
        }

        DrawRectangleRounded({x, y, DIE_SZ, DIE_SZ}, 0.2f, 8, bg);
        DrawRoundedBorder({x, y, DIE_SZ, DIE_SZ}, 0.2f, 8, 2.f, border);

        if (!showResult && !diceState.animating)
        {
            int qw = MeasureText("?", 24);
            DrawText("?", (int)(x + DIE_SZ / 2 - qw / 2), (int)(y + DIE_SZ / 2 - 12), 24, {150, 150, 150, 255});
            return;
        }

        // Pola titik dadu standar
        // Posisi titik: (col, row) dalam grid 3×3, dinormalisasi ke [0..1]
        struct Dot
        {
            float nx, ny;
        };
        static const Dot dots[6][6] = {
            {{.5f, .5f}},                                                                       // 1
            {{.25f, .25f}, {.75f, .75f}},                                                       // 2
            {{.25f, .25f}, {.5f, .5f}, {.75f, .75f}},                                           // 3
            {{.25f, .25f}, {.75f, .25f}, {.25f, .75f}, {.75f, .75f}},                           // 4
            {{.25f, .25f}, {.75f, .25f}, {.5f, .5f}, {.25f, .75f}, {.75f, .75f}},               // 5
            {{.25f, .25f}, {.75f, .25f}, {.25f, .5f}, {.75f, .5f}, {.25f, .75f}, {.75f, .75f}}, // 6
        };
        static const int dotCount[6] = {1, 2, 3, 4, 5, 6};

        int v = (val >= 1 && val <= 6) ? val : 1;
        for (int d = 0; d < dotCount[v - 1]; d++)
        {
            float dotX = x + dots[v - 1][d].nx * DIE_SZ;
            float dotY = y + dots[v - 1][d].ny * DIE_SZ;
            DrawCircle((int)dotX, (int)dotY, 5.f, dotCol);
        }
    };

    drawDie(d1x, dy, disp1, diceState.isDouble && !diceState.animating && showResult);
    drawDie(d2x, dy, disp2, diceState.isDouble && !diceState.animating && showResult);

    // ── Label hasil di bawah dadu ─────────────────────────────────────────
    if (showResult && !diceState.animating)
    {
        int total = diceState.val1 + diceState.val2;
        std::string lbl = std::to_string(diceState.val1) + " + " +
                          std::to_string(diceState.val2) + " = " +
                          std::to_string(total);
        int lw = MeasureText(lbl.c_str(), 13);
        DrawText(lbl.c_str(), (int)(cx - lw / 2), (int)(dy + DIE_SZ + 8), 13,
                 diceState.isDouble ? Color{255, 220, 50, 255} : Color{200, 200, 210, 255});

        if (diceState.isDouble)
        {
            const char *dblLbl = "DOUBLE!";
            int dw = MeasureText(dblLbl, 12);
            DrawText(dblLbl, (int)(cx - dw / 2), (int)(dy + DIE_SZ + 26), 12,
                     {255, 200, 50, 255});
        }
    }
}

// ─── Buy Dialog ──────────────────────────────────────────────────────────────

void GameScreen::triggerBuyDialog(int tileIdx)
{
    // Mode real — push BeliCommand akan dilakukan dari tombol dialog
    // Mode mock — cukup tampilkan dialog
    auto &curP = gameState.players[gameState.activePlayerIdx];
    auto &prop = gameState.properties[tileIdx];

    buyDialog.tileIdx = tileIdx;
    buyDialog.canAfford = (curP.money >= prop.price);
    buyDialog.visible = true;
}

void GameScreen::drawBuyDialog()
{
    if (!buyDialog.visible || buyDialog.tileIdx < 0)
        return;

    auto &prop = gameState.properties[buyDialog.tileIdx];
    auto &curP = gameState.players[gameState.activePlayerIdx];
    Color pCol = playerColors[gameState.activePlayerIdx];

    // ── Overlay gelap ────────────────────────────────────────────────────
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    // ── Panel utama ──────────────────────────────────────────────────────
    float pw = 420, ph = 300;
    float px = SCREEN_W / 2.f - pw / 2.f;
    float py = SCREEN_H / 2.f - ph / 2.f;

    DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {25, 27, 38, 255});
    DrawRectangleLinesEx({px, py, pw, ph}, 2.f, {80, 80, 130, 255});

    // ── Header dengan warna group ─────────────────────────────────────────
    Color hdrCol = prop.colorGroup.empty()
                       ? Color{50, 52, 70, 255}
                       : getGroupColor(prop.colorGroup);
    DrawRectangle((int)px, (int)py, (int)pw, 52, hdrCol);

    std::string propName = prop.name.empty()
                               ? TILE_DEFS[buyDialog.tileIdx].code
                               : prop.name;
    int nw = MeasureText(propName.c_str(), 18);
    DrawText(propName.c_str(), (int)(px + pw / 2 - nw / 2), (int)(py + 8), 18, WHITE);

    int gw = MeasureText(prop.colorGroup.c_str(), 11);
    DrawText(prop.colorGroup.c_str(), (int)(px + pw / 2 - gw / 2), (int)(py + 32), 11,
             {255, 255, 255, 180});

    // ── Info pemain & harga ───────────────────────────────────────────────
    float ry = py + 68;

    DrawText(curP.username.c_str(), (int)(px + 20), (int)ry, 13, pCol);

    // Saldo pemain
    std::string balStr = "Saldo: M" + std::to_string(curP.money);
    int bw = MeasureText(balStr.c_str(), 12);
    DrawText(balStr.c_str(), (int)(px + pw - 20 - bw), (int)ry, 12,
             buyDialog.canAfford ? Color{100, 220, 100, 255} : Color{220, 80, 80, 255});

    DrawLine((int)(px + 16), (int)(ry + 18), (int)(px + pw - 16), (int)(ry + 18),
             {60, 60, 90, 255});

    ry += 28;

    // Harga beli
    DrawText("Harga Beli", (int)(px + 20), (int)ry, 12, {140, 140, 180, 255});
    std::string priceStr = "M" + std::to_string(prop.price);
    int pw2 = MeasureText(priceStr.c_str(), 18);
    DrawText(priceStr.c_str(), (int)(px + pw - 20 - pw2), (int)(ry - 3), 18, WHITE);
    // cout << "DEBUG: GAMEMODE " << (isRealMode() ? "REAL" : "MOCK") << endl;
    ry += 28;

    // Sisa saldo setelah beli
    int remaining = curP.money - prop.price;
    DrawText("Sisa saldo", (int)(px + 20), (int)ry, 11, {120, 120, 160, 255});
    std::string remStr = "M" + std::to_string(remaining);
    Color remCol = remaining >= 0 ? Color{100, 200, 100, 255} : Color{220, 80, 80, 255};
    int rw = MeasureText(remStr.c_str(), 12);
    DrawText(remStr.c_str(), (int)(px + pw - 20 - rw), (int)ry, 12, remCol);

    ry += 26;

    // Peringatan jika tidak mampu
    if (!buyDialog.canAfford)
    {
        const char *warn = "Saldo tidak cukup — properti akan dilelang";
        int ww = MeasureText(warn, 10);
        DrawText(warn, (int)(px + pw / 2 - ww / 2), (int)ry, 10, {220, 100, 80, 255});
        ry += 18;
    }

    // ── Tombol BELI & SKIP ────────────────────────────────────────────────
    float btnY = py + ph - 56;
    float btnW = pw / 2.f - 24;
    Vector2 mouse = GetMousePosition();

    // Tombol BELI
    Rectangle buyBtn = {px + 16, btnY, btnW, 40};
    bool buyHover = CheckCollisionPointRec(mouse, buyBtn) && buyDialog.canAfford;
    bool buyDisabled = !buyDialog.canAfford;
    Color buyBg = buyDisabled ? Color{40, 42, 54, 255}
                  : buyHover  ? Color{60, 180, 90, 255}
                              : Color{40, 140, 70, 255};
    Color buyBorder = buyDisabled ? Color{60, 60, 80, 255} : Color{80, 200, 110, 255};
    DrawRectangleRec(buyBtn, buyBg);
    DrawRectangleLinesEx(buyBtn, 1.5f, buyBorder);
    const char *buyLbl = buyDisabled ? "TIDAK MAMPU" : "BELI";
    int blw = MeasureText(buyLbl, 13);
    DrawText(buyLbl, (int)(buyBtn.x + btnW / 2 - blw / 2), (int)(btnY + 13), 13,
             buyDisabled ? Color{80, 80, 100, 255} : WHITE);

    // Tombol SKIP / LELANG
    Rectangle skipBtn = {px + pw / 2 + 8, btnY, btnW, 40};
    bool skipHover = CheckCollisionPointRec(mouse, skipBtn);
    Color skipBg = skipHover ? Color{180, 100, 50, 255} : Color{120, 60, 30, 255};
    DrawRectangleRec(skipBtn, skipBg);
    DrawRectangleLinesEx(skipBtn, 1.5f, {200, 130, 80, 255});
    const char *skipLbl = "SKIP (LELANG)";
    int slw = MeasureText(skipLbl, 11);
    DrawText(skipLbl, (int)(skipBtn.x + btnW / 2 - slw / 2), (int)(btnY + 14), 11, WHITE);

    // ── Handle klik ──────────────────────────────────────────────────────
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (buyHover && !buyDisabled)
        {
            // Mode real: push BeliCommand(buy=true)
            if (guiManager && guiManager->getGameMaster())
            {
                GameMaster *gm = guiManager->getGameMaster();
                Player *player = gm->getState().getCurrPlayer();
                Bank *bank = gm->getState().getBank();
                // Cari Property* dari board
                Board *board = gm->getState().getBoard();
                Tile *tile = board->getTile(buyDialog.tileIdx);
                auto *pt = dynamic_cast<PropertyTile *>(tile);
                if (player && bank && pt)
                {
                    guiManager->pushCommand(
                        new BeliCommand(player, pt->getProperty(), bank, true));
                }
            }
            else
            {
                // Mode mock
                auto &p = gameState.players[gameState.activePlayerIdx];
                auto &pr = gameState.properties[buyDialog.tileIdx];
                p.money -= pr.price;
                pr.owner = gameState.activePlayerIdx;

                gameState.logger.addLog(gameState.currentTurn, p.username,
                                        "BELI", "Beli " + pr.name + ": -M" + std::to_string(pr.price));
            }
            buyDialog.visible = false;
        }
        else if (skipHover)
        {
            // Mode real: push BeliCommand(buy=false)
            if (guiManager && guiManager->getGameMaster())
            {
                GameMaster *gm = guiManager->getGameMaster();
                Player *player = gm->getState().getCurrPlayer();
                Bank *bank = gm->getState().getBank();
                Board *board = gm->getState().getBoard();
                Tile *tile = board->getTile(buyDialog.tileIdx);
                auto *pt = dynamic_cast<PropertyTile *>(tile);
                if (player && bank && pt)
                {
                    guiManager->pushCommand(
                        new BeliCommand(player, pt->getProperty(), bank, false));
                }
            }
            else
            {
                // Mode mock — skip, tidak ada lelang
                auto &p = gameState.players[gameState.activePlayerIdx];
                auto &pr = gameState.properties[buyDialog.tileIdx];
                gameState.logger.addLog(gameState.currentTurn, p.username,
                                        "BELI", "Skip " + pr.name + " → lelang");
            }
            buyDialog.visible = false;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void GameScreen::initMockLogs()
{
    gameState.logger.addLog(15, "Uname1", "DADU", "Lempar: 4+5=9, mendarat di BDG");
    gameState.logger.addLog(15, "Uname1", "SEWA", "Bayar sewa ke Uname2: -M 260");
    gameState.logger.addLog(14, "Uname2", "BANGUN", "Bangun rumah di BDG: -M 200");
    gameState.logger.addLog(14, "Uname3", "KARTU", "Dapat Kesempatan: maju 3 petak");
    gameState.logger.addLog(14, "Uname3", "DADU", "Lempar: 2+1=3, mendarat di FES");
    gameState.logger.addLog(13, "Uname4", "GADAI", "Gadai MGL: +M 100");
    gameState.logger.addLog(13, "Uname1", "BELI", "Beli JKT: -M 350");
    gameState.logger.addLog(13, "Uname2", "DADU", "Lempar: 6+6=12 (double!), mendarat di BDG");
    gameState.logger.addLog(12, "Uname3", "BAYAR", "Bayar PPH flat: -M 150");
    gameState.logger.addLog(12, "Uname4", "LELANG", "Menang lelang GRT: M 45");
    gameState.logger.addLog(12, "Uname1", "DADU", "Lempar: 3+4=7, mendarat di GBR");
    gameState.logger.addLog(11, "Uname2", "KARTU", "Dapat Dana Umum: +M 100");
    gameState.logger.addLog(11, "Uname3", "BANGUN", "Bangun rumah di MKS: -M 150");
    gameState.logger.addLog(11, "Uname4", "DADU", "Lempar: 1+2=3, mendarat di TSK");
    gameState.logger.addLog(10, "Uname1", "SEWA", "Bayar sewa ke Uname3: -M 700");
}

std::string GameScreen::getActionIcon(const std::string &action)
{
    if (action == "DADU")
        return "[D]";
    if (action == "BELI")
        return "[B]";
    if (action == "SEWA")
        return "[$]";
    if (action == "BANGUN")
        return "[H]";
    if (action == "GADAI")
        return "[G]";
    if (action == "KARTU")
        return "[K]";
    if (action == "LELANG")
        return "[L]";
    if (action == "BAYAR")
        return "[P]";
    return "[?]";
}

Color GameScreen::getActionColor(const std::string &action)
{
    if (action == "DADU")
        return {100, 180, 240, 255};
    if (action == "BELI")
        return {100, 200, 120, 255};
    if (action == "SEWA")
        return {220, 100, 100, 255};
    if (action == "BANGUN")
        return {100, 220, 120, 255};
    if (action == "GADAI")
        return {220, 160, 60, 255};
    if (action == "KARTU")
        return {180, 120, 220, 255};
    if (action == "LELANG")
        return {240, 200, 60, 255};
    if (action == "BAYAR")
        return {220, 80, 80, 255};
    return {180, 180, 180, 255};
}

void GameScreen::drawLogPopup()
{
    if (!showLogPopup)
        return;

    // ── Pilih sumber log ─────────────────────────────────────────────────
    // Jika mode real, ambil log dari TransactionLogger milik GameState.
    // Jika mode mock, tetap pakai gameState.logger.
    TransactionLogger *logSrc = &gameState.logger;
    if (isRealMode())
    {
        TransactionLogger *realLogger =
            guiManager->getGameMaster()->getState().getLogger();
        if (realLogger)
            logSrc = realLogger;
    }

    // Ambil log sesuai jumlah yang diminta
    std::vector<LogEntry> logs;
    if (logShowN > 0)
        logs = logSrc->getLastLogs(logShowN);
    else
        logs = logSrc->getLogs();

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    float pw = 700, ph = 540;
    float px = SCREEN_W / 2.f - pw / 2.f;
    float py = SCREEN_H / 2.f - ph / 2.f;

    // Panel background
    DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {25, 27, 38, 255});
    DrawRectangleLinesEx({px, py, pw, ph}, 1.5f, {80, 80, 120, 255});

    // Header bar
    DrawRectangle((int)px, (int)py, (int)pw, 44, {35, 37, 52, 255});
    DrawRectangleLinesEx({px, py, pw, 44}, 1, {80, 80, 120, 255});
    DrawText("LOG TRANSAKSI", (int)(px + 16), (int)(py + 14), 15, {180, 180, 220, 255});

    // ── N input box ───────────────────────────────────────────────────────────
    float ibX = px + pw - 180;
    float ibY = py + 10;
    float ibW = 100, ibH = 28;
    Rectangle inputBox = {ibX, ibY, ibW, ibH};

    bool hover = CheckCollisionPointRec(GetMousePosition(), inputBox);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        logNFocused = hover;

    // Draw box
    DrawRectangleRec(inputBox, logNFocused ? Color{40, 45, 65, 255} : Color{30, 32, 48, 255});
    DrawRectangleLinesEx(inputBox, 1,
                         logNFocused ? Color{120, 160, 240, 255} : Color{70, 70, 100, 255});

    // Label
    DrawText("Tampilkan:", (int)(ibX - MeasureText("Tampilkan:", 11) - 8),
             (int)(ibY + 8), 11, {140, 140, 180, 255});

    // Handle keyboard input
    if (logNFocused)
    {
        // Backspace
        if (IsKeyPressed(KEY_BACKSPACE) && !logNInput.empty())
            logNInput.pop_back();

        // Hanya angka, max 3 digit
        int ch = GetCharPressed();
        while (ch > 0)
        {
            if (ch >= '0' && ch <= '9' && logNInput.size() < 3)
                logNInput += (char)ch;
            ch = GetCharPressed();
        }

        // Enter → apply
        if (IsKeyPressed(KEY_ENTER))
        {
            logShowN = logNInput.empty() ? 0 : std::stoi(logNInput);
            logNFocused = false;
        }
    }

    // Display text
    std::string inputDisplay = logNInput.empty() ? "Semua" : logNInput;
    if (logNInput.empty() && logNFocused)
        inputDisplay = "";
    int tw = MeasureText(inputDisplay.c_str(), 12);
    DrawText(inputDisplay.c_str(), (int)(ibX + ibW / 2 - tw / 2), (int)(ibY + 7), 12,
             logNInput.empty() ? Color{100, 100, 140, 255} : WHITE);

    // Cursor blink
    if (logNFocused && (int)(GetTime() * 2) % 2 == 0)
        DrawText("|", (int)(ibX + ibW / 2 + tw / 2 + 2), (int)(ibY + 7), 12, WHITE);

    // Close button
    Rectangle closeBtn = {px + pw - 36, py + 8, 28, 28};
    bool closeHover = CheckCollisionPointRec(GetMousePosition(), closeBtn);
    DrawRectangleRec(closeBtn, closeHover ? Color{180, 60, 60, 255} : Color{120, 40, 40, 255});
    DrawRectangleLinesEx(closeBtn, 1, {200, 80, 80, 255});
    DrawText("X", (int)(closeBtn.x + 9), (int)(closeBtn.y + 8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && closeHover)
        showLogPopup = false;

    // Column headers
    float headerY = py + 50;
    DrawRectangle((int)px, (int)headerY, (int)pw, 24, {35, 37, 52, 255});
    DrawText("TURN", (int)(px + 16), (int)(headerY + 6), 11, {120, 120, 160, 255});
    DrawText("AKSI", (int)(px + 80), (int)(headerY + 6), 11, {120, 120, 160, 255});
    DrawText("PEMAIN", (int)(px + 150), (int)(headerY + 6), 11, {120, 120, 160, 255});
    DrawText("DETAIL", (int)(px + 270), (int)(headerY + 6), 11, {120, 120, 160, 255});
    DrawLine((int)px, (int)(headerY + 24), (int)(px + pw), (int)(headerY + 24), {60, 60, 90, 255});

    // Filter logs (newest first)
    auto all = (logShowN == 0)
                   ? logSrc->getLogs()
                   : logSrc->getLastLogs(logShowN);
    std::vector<LogEntry> display(all.rbegin(), all.rend());

    // Scroll input
    float listY1 = headerY + 26;
    float listH = ph - (listY1 - py) - 10;
    float rowH = 32.f;
    float totalH = display.size() * rowH;

    Rectangle listArea = {px, listY1, pw, listH};
    if (CheckCollisionPointRec(GetMousePosition(), listArea))
    {
        float wheel = GetMouseWheelMove();
        logScrollY -= wheel * 40.f;
        logScrollY = std::max(0.f, std::min(logScrollY, std::max(0.f, totalH - listH)));
    }

    // Draw rows
    BeginScissorMode((int)px, (int)listY1, (int)pw, (int)listH);
    for (int i = 0; i < (int)display.size(); i++)
    {
        auto &e = display[i];
        float ry = listY1 + i * rowH - logScrollY;
        if (ry + rowH < listY1 || ry > listY1 + listH)
            continue;

        // Alternating row bg
        Color rowBg = (i % 2 == 0) ? Color{28, 30, 42, 255} : Color{32, 34, 48, 255};
        DrawRectangle((int)px, (int)ry, (int)pw, (int)rowH - 1, rowBg);

        // Turn
        DrawText(("T" + std::to_string(e.turn)).c_str(),
                 (int)(px + 16), (int)(ry + 9), 11, {120, 120, 160, 255});

        // Action badge
        Color ac = getActionColor(e.actionType);
        std::string icon = getActionIcon(e.actionType);
        DrawRectangle((int)(px + 70), (int)(ry + 6), 60, 20, {ac.r, ac.g, ac.b, 50});
        DrawRectangleLinesEx({px + 70, ry + 6, 60, 20}, 1, {ac.r, ac.g, ac.b, 150});
        int iw = MeasureText(icon.c_str(), 10);
        DrawText(icon.c_str(), (int)(px + 100 - iw / 2), (int)(ry + 9), 10, ac);

        // Username (warna per player)
        Color uc = {180, 180, 180, 255};
        for (int p = 0; p < (int)gameState.players.size(); p++)
            if (gameState.players[p].username == e.username)
            {
                uc = playerColors[p];
                break;
            }
        DrawText(e.username.c_str(), (int)(px + 150), (int)(ry + 9), 11, uc);

        // Detail
        DrawText(e.detail.c_str(), (int)(px + 270), (int)(ry + 9), 11, {200, 200, 215, 255});
    }
    EndScissorMode();

    // Scrollbar
    if (totalH > listH)
    {
        float sbH = listH * (listH / totalH);
        float sbY = listY1 + (logScrollY / totalH) * listH;
        DrawRectangle((int)(px + pw - 6), (int)listY1, 4, (int)listH, {40, 42, 58, 255});
        DrawRectangle((int)(px + pw - 6), (int)sbY, 4, (int)sbH, {100, 100, 160, 255});
    }

    // Empty state
    if (display.empty())
    {
        int tw = MeasureText("Belum ada log.", 14);
        DrawText("Belum ada log.",
                 (int)(px + pw / 2 - tw / 2), (int)(listY1 + listH / 2 - 10),
                 14, {100, 100, 140, 255});
    }
}