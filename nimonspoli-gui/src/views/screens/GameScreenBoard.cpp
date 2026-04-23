#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include <cmath>
#include <string>

extern const TileDef TILE_DEFS[40];

// ─── Internal helper ──────────────────────────────────────────────────────────
static void DrawRoundedBorder(Rectangle rec, float roundness, int segments,
                              float thick, Color color)
{
    for (float i = 0.f; i < thick; i += 0.5f) {
        Rectangle r = {rec.x - i, rec.y - i,
                       rec.width + i * 2, rec.height + i * 2};
        DrawRectangleRoundedLines(r, roundness, segments, color);
    }
}

// Helper: gambar border rectangle yang ikut rotasi tile
// DrawRectangleLinesEx tidak ikut rotasi, sehingga tile LEFT/RIGHT
// akan tampil border dengan dimensi terbalik. Solusi: transform 4 sudut manual.
static void DrawRotatedBorder(float cx, float cy, float tw, float th,
                               float rotation, float thick, Color color)
{
    float rad  = rotation * DEG2RAD;
    float cosR = cosf(rad), sinR = sinf(rad);

    float hw = tw / 2.f, hh = th / 2.f;
    Vector2 corners[4] = {
        {-hw, -hh}, { hw, -hh},
        { hw,  hh}, {-hw,  hh}
    };

    Vector2 world[4];
    for (int i = 0; i < 4; i++) {
        world[i] = {
            cx + corners[i].x * cosR - corners[i].y * sinR,
            cy + corners[i].x * sinR + corners[i].y * cosR
        };
    }

    for (int i = 0; i < 4; i++)
        DrawLineEx(world[i], world[(i + 1) % 4], thick, color);
}

// ─── Tile geometry ────────────────────────────────────────────────────────────
Vector2 GameScreen::getTileCenter(int idx)
{
    auto& td = TILE_DEFS[idx];
    float x = 0, y = 0;
    float boardSz = CORNER_SZ + 9 * TILE_W + CORNER_SZ;

    if (td.side == "BOTTOM") {
        if (td.corner) {
            x = (idx == 0) ? boardX + boardSz - CORNER_SZ / 2.f
                           : boardX + CORNER_SZ / 2.f;
            y = boardY + boardSz - CORNER_SZ / 2.f;
        } else {
            int slot = 9 - idx;
            x = boardX + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
            y = boardY + boardSz - TILE_H / 2.f;
        }
    } else if (td.side == "LEFT") {
        int slot = 8 - (idx - 11);
        x = boardX + TILE_H / 2.f;
        y = boardY + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
    } else if (td.side == "TOP") {
        if (td.corner) {
            x = (idx == 20) ? boardX + CORNER_SZ / 2.f
                            : boardX + boardSz - CORNER_SZ / 2.f;
            y = boardY + CORNER_SZ / 2.f;
        } else {
            int slot = idx - 21;
            x = boardX + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
            y = boardY + TILE_H / 2.f;
        }
    } else {
        int slot = idx - 31;
        x = boardX + boardSz - TILE_H / 2.f;
        y = boardY + CORNER_SZ + slot * TILE_W + TILE_W / 2.f;
    }
    return {x, y};
}

Rectangle GameScreen::getTileRect(int idx)
{
    Vector2 c  = getTileCenter(idx);
    auto&   td = TILE_DEFS[idx];
    float hw, hh;
    if (td.corner) {
        hw = CORNER_SZ / 2.f; hh = CORNER_SZ / 2.f;
    } else if (td.side == "BOTTOM" || td.side == "TOP") {
        hw = TILE_H / 2.f; hh = TILE_W / 2.f;
    } else {
        hw = TILE_W / 2.f; hh = TILE_H / 2.f;
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
    auto& td   = TILE_DEFS[idx];
    auto& prop = gameState.properties[idx];
    float tw   = td.corner ? CORNER_SZ : TILE_W;
    float th   = td.corner ? CORNER_SZ : TILE_H;

    auto it = tileTextures.find(td.code);
    if (it != tileTextures.end()) {
        Texture2D& tex = it->second;
        Rectangle src  = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dst  = {cx, cy, tw, th};
        DrawTexturePro(tex, src, dst, {tw/2.f, th/2.f}, rotation, WHITE);
    } else {
        Rectangle r = {cx - tw/2.f, cy - th/2.f, tw, th};
        DrawRectangleRec(r, DARKGRAY);
        DrawRectangleLinesEx(r, 1, BLACK);
        int fw = MeasureText(td.code.c_str(), 9);
        DrawText(td.code.c_str(), (int)(cx - fw/2), (int)(cy - 5), 9, WHITE);
    }

    if (prop.festivalMult > 1) {
        float pulse = 0.7f + 0.3f * sinf(glowTimer * 4.f);
        float thick = 1.5f * log2f((float)prop.festivalMult);
        Color rc;
        if      (prop.festivalMult == 2) rc = {239, 159,  39, (unsigned char)(200 * pulse)};
        else if (prop.festivalMult == 4) rc = {186, 117,  23, (unsigned char)(220 * pulse)};
        else                             rc = {133,  79,  11, (unsigned char)(240 * pulse)};
        // Gambar di luar tile dengan offset thick, ikut rotasi
        DrawRotatedBorder(cx, cy, tw + thick*2, th + thick*2, rotation, thick, rc);
        std::string lbl = "x" + std::to_string(prop.festivalMult);
        DrawText(lbl.c_str(), (int)(cx-tw/2.f+2), (int)(cy-th/2.f+2), 10, rc);
    }

    if (prop.owner >= 0 && prop.type == "STREET") {
        drawBuildingStrip(cx, cy, rotation, prop.buildings, playerColors[prop.owner]);
        // Border ikut rotasi tile — fix bug marker terbalik di LEFT/RIGHT
        DrawRotatedBorder(cx, cy, tw, th, rotation, 5.f, playerColors[prop.owner]);
        if (prop.mortgaged) {
            DrawRectanglePro({cx, cy, tw, th}, {tw/2.f, th/2.f}, rotation, {0, 0, 0, 140});
            int fw = MeasureText("GADAI", 10);
            DrawText("GADAI", (int)(cx - fw/2), (int)(cy - 5), 10, RED);
        }
    } else if (prop.owner >= 0) {
        // Railroad / Utility
        DrawRotatedBorder(cx, cy, tw, th, rotation, 3.f, playerColors[prop.owner]);
    }
}

// ─── Building strip ───────────────────────────────────────────────────────────
void GameScreen::drawBuildingStrip(float cx, float cy, float rotation,
                                   int buildings, Color ownerColor)
{
    float sw = STRIP_W, sh = STRIP_H;
    float localY = TILE_H/2.f - sh/2.f;
    float rad = rotation * DEG2RAD;
    float cosR = cosf(rad), sinR = sinf(rad);
    float sx = cx - localY * sinR;
    float sy = cy + localY * cosR;
    DrawRectanglePro({sx, sy, sw, sh}, {sw/2.f, sh/2.f}, rotation, ownerColor);

    if (buildings == 0) return;

    float bldLocalY = localY - sh;
    bool  isHotel   = (buildings == 5);
    if (isHotel) {
        float hw = sw * 0.85f, hh = sh * 0.7f;
        float bsx = cx - bldLocalY * sinR;
        float bsy = cy + bldLocalY * cosR;
        DrawRectanglePro({bsx, bsy, hw, hh}, {hw/2.f, hh/2.f}, rotation, ownerColor);
        DrawRectanglePro({bsx, bsy, hw, hh}, {hw/2.f, hh/2.f}, rotation, {0,0,0,80});
    } else {
        float sqSz   = sh * 0.75f;
        float totalW = sw * 0.9f;
        float gap    = (buildings > 1) ? (totalW - buildings * sqSz) / (buildings - 1) : 0;
        float startX = -(totalW/2.f) + sqSz/2.f;
        for (int b = 0; b < buildings; b++) {
            float lx  = startX + b * (sqSz + gap);
            float bsx = cx + lx * cosR - bldLocalY * sinR;
            float bsy = cy + lx * sinR + bldLocalY * cosR;
            DrawRectanglePro({bsx, bsy, sqSz, sqSz}, {sqSz/2.f, sqSz/2.f}, rotation, ownerColor);
            DrawRectanglePro({bsx, bsy, sqSz, sqSz}, {sqSz/2.f, sqSz/2.f}, rotation, {0,0,0,60});
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
    if (onTile.empty()) return;

    float r = 8.f, sp = r * 2.2f;
    float startX = cx - (onTile.size() * sp) / 2.f + r;
    for (int i = 0; i < (int)onTile.size(); i++) {
        int   pi = onTile[i];
        float px = startX + i * sp;
        DrawCircle((int)px, (int)cy, r + 3, WHITE);
        DrawCircle((int)px, (int)cy, r, playerColors[pi]);
        std::string num = std::to_string(pi + 1);
        int fw = MeasureText(num.c_str(), 9);
        DrawText(num.c_str(), (int)(px - fw/2.f), (int)(cy - 5), 9, WHITE);
    }
}

// ─── Draw board ───────────────────────────────────────────────────────────────
void GameScreen::drawBoard()
{
    // Fallback: isi playerVisuals dari gameState jika belum ada
    if (playerVisuals.empty() && !gameState.players.empty()) {
        playerVisuals.resize(gameState.players.size());
        for (int i = 0; i < (int)gameState.players.size(); i++) {
            float pos = (float)gameState.players[i].position;
            playerVisuals[i].currentTileIdx = pos;
            playerVisuals[i].targetTileIdx  = pos;
        }
    }

    float sz = CORNER_SZ + 9 * TILE_W + CORNER_SZ;
    DrawRectangle((int)boardX, (int)boardY, (int)sz, (int)sz, {230, 225, 210, 255});

    // ── Gambar semua tile ────────────────────────────────────────────────
    for (int i = 0; i < 40; i++) {
        Vector2 c       = getTileCenter(i);
        int     activeP = gameState.activePlayerIdx;
        if (activeP >= 0 && activeP < (int)playerVisuals.size()) {
            if (i == (int)playerVisuals[activeP].targetTileIdx) {
                Rectangle r = getTileRect(i);
                DrawRectangleRec(r, {255, 255, 0,
                    (unsigned char)(50 + 30 * sinf(glowTimer * 6))});
            }
        }
        float rot = (TILE_DEFS[i].side == "BOTTOM") ?   0
                  : (TILE_DEFS[i].side == "LEFT")   ? 270
                  : (TILE_DEFS[i].side == "TOP")    ? 180
                                                    :  90;
        drawTile(i, c.x, c.y, rot);
    }

    // ── Gambar pion per tile (tumpuk ke samping sesuai orientasi) ────────
    constexpr float RADIUS  = 9.f;
    constexpr float OVERLAP = 0.5f;   // tumpuk 50% diameter
    float spacing = RADIUS * 2.f * (1.f - OVERLAP);

    for (int tileIdx = 0; tileIdx < 40; tileIdx++) {
        // Kumpulkan pemain yang sedang di tile ini (pakai posisi visual/animasi)
        std::vector<int> onTile;
        for (int p = 0; p < (int)playerVisuals.size(); p++) {
            int cur = (int)playerVisuals[p].currentTileIdx % 40;
            if (cur == tileIdx) onTile.push_back(p);
        }
        if (onTile.empty()) continue;

        Vector2 center = getTileCenter(tileIdx);
        int     n      = (int)onTile.size();

        // Arah tumpukan: horizontal untuk BOTTOM/TOP, vertikal untuk LEFT/RIGHT
        bool isVertical = (TILE_DEFS[tileIdx].side == "LEFT" ||
                           TILE_DEFS[tileIdx].side == "RIGHT");

        float totalSpan = RADIUS * 2.f + spacing * (n - 1);
        float startX = center.x - (isVertical ? 0.f : totalSpan / 2.f - RADIUS);
        float startY = center.y - (isVertical ? totalSpan / 2.f - RADIUS : 0.f);

        for (int i = 0; i < n; i++) {
            int p = onTile[i];

            // Hitung posisi pion ini — interpolasi antara tile saat ini dan berikutnya
            float visualIdx = playerVisuals[p].currentTileIdx;
            int   tileA     = (int)visualIdx % 40;
            int   tileB     = (tileA + 1) % 40;
            float frac      = visualIdx - (float)(int)visualIdx;
            Vector2 posA    = getTileCenter(tileA);
            Vector2 posB    = getTileCenter(tileB);

            // Posisi animasi (interpolasi antar tile)
            float animX = posA.x + (posB.x - posA.x) * frac;
            float animY = posA.y + (posB.y - posA.y) * frac;

            // Offset tumpukan — hanya berlaku kalau sudah sampai di tile tujuan
            bool arrived = ((int)playerVisuals[p].currentTileIdx % 40 ==
                            (int)playerVisuals[p].targetTileIdx % 40);
            float px, py;
            if (arrived) {
                px = isVertical ? center.x : startX + i * spacing;
                py = isVertical ? startY + i * spacing : center.y;
            } else {
                // Sedang animasi — pakai posisi interpolasi tanpa offset tumpukan
                px = animX;
                py = animY;
            }

            bool  isActive = (p == gameState.activePlayerIdx);
            float r        = isActive ? RADIUS + 2.f : RADIUS;

            // Border putih
            DrawCircle((int)px, (int)py, r + 2, WHITE);
            // Warna pemain
            DrawCircle((int)px, (int)py, r, playerColors[p]);

            // Nomor pemain
            std::string num = std::to_string(p + 1);
            int fw = MeasureText(num.c_str(), 8);
            DrawText(num.c_str(),
                     (int)(px - fw / 2.f), (int)(py - 5), 8, WHITE);

            // Ring glow untuk pemain aktif
            if (isActive) {
                auto gCol = playerColors[p];
                DrawCircleLines((int)px, (int)py, r + 4,
                    {gCol.r, gCol.g, gCol.b,
                     (unsigned char)(150 + 100 * sinf(glowTimer * 5.f))});
            }
        }
    }

    drawCenterArea();
}

// ─── Center area ─────────────────────────────────────────────────────────────
void GameScreen::drawCenterArea()
{
    float cx1 = boardX + CORNER_SZ;
    float cy1 = boardY + CORNER_SZ;
    float cw  = 9 * TILE_W;
    float ch  = 9 * TILE_W;
    float ccx = cx1 + cw/2.f;
    float ccy = cy1 + ch/2.f;

    BeginScissorMode((int)cx1, (int)cy1, (int)cw, (int)ch);
    float ox = zoomOffset.x, oy = zoomOffset.y;

    DrawText("NIMONSPOLI",
             (int)(ccx+ox-60), (int)(ccy+oy-80),
             (int)(36*zoomLevel), {80,60,40,200});
    std::string ts = "Turn " + std::to_string(gameState.currentTurn) +
                     " / "   + std::to_string(gameState.maxTurn);
    DrawText(ts.c_str(),
             (int)(ccx+ox-50), (int)(ccy+oy-30),
             (int)(18*zoomLevel), {100,80,60,200});

    float cardW  = 220 * zoomLevel;
    float cardH  = (deckKSP.id != 0)
                      ? cardW * ((float)deckKSP.height / deckKSP.width)
                      : cardW * 1.4f;
    float margin = 100 * zoomLevel;
    float kspCX  = cx1+ox+margin+cardW/2.f;
    float kspCY  = cy1+oy+margin+30+cardH/2.f;
    float kspRot = 135.f;
    float dnuCX  = cx1+ox+cw-margin-cardW/2.f;
    float dnuCY  = cy1+oy+ch-margin-30-cardH/2.f;
    float dnuRot = -45.f;

    if (kspGlowing) {
        auto a = (unsigned char)(160+80*sinf(glowTimer*5.f));
        DrawRectanglePro({kspCX,kspCY,cardW+20,cardH+20},{(cardW+20)/2.f,(cardH+20)/2.f},kspRot,{255,220,0,a});
    }
    if (dnuGlowing) {
        auto a = (unsigned char)(160+80*sinf(glowTimer*5.f));
        DrawRectanglePro({dnuCX,dnuCY,cardW+20,cardH+20},{(cardW+20)/2.f,(cardH+20)/2.f},dnuRot,{220,50,50,a});
    }

    if (deckKSP.id != 0)
        DrawTexturePro(deckKSP,{0,0,(float)deckKSP.width,(float)deckKSP.height},
                       {kspCX,kspCY,cardW,cardH},{cardW/2.f,cardH/2.f},kspRot,WHITE);
    else {
        DrawRectanglePro({kspCX,kspCY,cardW,cardH},{cardW/2.f,cardH/2.f},kspRot,{255,180,0,200});
        DrawText("KSP",(int)(kspCX-10),(int)(kspCY-8),14,WHITE);
    }
    if (deckDNU.id != 0)
        DrawTexturePro(deckDNU,{0,0,(float)deckDNU.width,(float)deckDNU.height},
                       {dnuCX,dnuCY,cardW,cardH},{cardW/2.f,cardH/2.f},dnuRot,WHITE);
    else {
        DrawRectanglePro({dnuCX,dnuCY,cardW,cardH},{cardW/2.f,cardH/2.f},dnuRot,{180,50,50,200});
        DrawText("DNU",(int)(dnuCX-10),(int)(dnuCY-8),14,WHITE);
    }

    if (kspGlowing)
        DrawText("Klik untuk ambil!",(int)(kspCX-cardW/2.f),(int)(kspCY+cardH/2.f+8),
                 (int)(11*zoomLevel),{200,160,0,255});
    if (dnuGlowing)
        DrawText("Klik untuk ambil!",(int)(dnuCX-cardW/2.f),(int)(dnuCY+cardH/2.f+8),
                 (int)(11*zoomLevel),{200,60,60,255});

    EndScissorMode();
}

// ─── Property popup ───────────────────────────────────────────────────────────
void GameScreen::drawPopup()
{
    if (!showPopup || selectedTile < 0) return;
    auto& prop = gameState.properties[selectedTile];

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 140});
    float pw = 300, ph = 480;
    float px = SCREEN_W/2.f - pw/2.f, py = SCREEN_H/2.f - ph/2.f;

    DrawRectangle((int)px,(int)py,(int)pw,(int)ph,{240,238,230,255});
    DrawRectangleLinesEx({px,py,pw,ph},2,{80,70,60,255});

    Color hdr = prop.colorGroup.empty() ? DARKGRAY : getGroupColor(prop.colorGroup);
    DrawRectangle((int)px,(int)py,(int)pw,60,hdr);
    std::string name = prop.name.empty() ? TILE_DEFS[selectedTile].code : prop.name;
    int nw = MeasureText(name.c_str(),18);
    DrawText(name.c_str(),(int)(px+pw/2-nw/2),(int)(py+8),18,WHITE);
    int gw = MeasureText(prop.colorGroup.c_str(),12);
    DrawText(prop.colorGroup.c_str(),(int)(px+pw/2-gw/2),(int)(py+30),12,{255,255,255,200});

    std::string status = "BANK"; Color sc = GRAY;
    if (prop.owner >= 0) {
        status = gameState.players[prop.owner].username;
        sc     = playerColors[prop.owner];
        if (prop.mortgaged) { status += " [GADAI]"; sc = RED; }
    }
    DrawText(status.c_str(),(int)(px+10),(int)(py+68),13,sc);

    float ry = py + 90;
    auto row = [&](const char* key, const std::string& val, Color vc = {40,40,40,255}) {
        DrawText(key,(int)(px+10),(int)ry,12,{120,110,100,255});
        int vw = MeasureText(val.c_str(),12);
        DrawText(val.c_str(),(int)(px+pw-10-vw),(int)ry,12,vc);
        DrawLine((int)(px+10),(int)(ry+16),(int)(px+pw-10),(int)(ry+16),{200,195,185,255});
        ry += 22;
    };

    if (prop.type == "STREET") {
        row("Harga beli",   "M "+std::to_string(prop.price));
        row("Nilai gadai",  "M "+std::to_string(prop.mortgageVal));
        ry += 6;
        DrawText("Tabel Sewa",(int)(px+10),(int)ry,11,{100,90,80,255}); ry += 18;
        row("Tanah kosong", "M "+std::to_string(prop.rentL0));
        row("Monopoli (x2)","M "+std::to_string(prop.rentL0*2));
        row("1 rumah",      "M "+std::to_string(prop.rentL1));
        row("2 rumah",      "M "+std::to_string(prop.rentL2));
        row("3 rumah",      "M "+std::to_string(prop.rentL3));
        row("4 rumah",      "M "+std::to_string(prop.rentL4));
        row("Hotel",        "M "+std::to_string(prop.rentL5));
        ry += 6;
        if (prop.festivalMult > 1)
            row("Festival","x"+std::to_string(prop.festivalMult)+
                " ("+std::to_string(prop.festivalDur)+" giliran)",{200,140,0,255});
        row("Upgrade rumah","M "+std::to_string(prop.houseUpg));
        row("Upgrade hotel","M "+std::to_string(prop.hotelUpg));
    } else {
        row("Jenis", prop.type);
    }

    float btnY = py+ph-50, btnW = pw/3.f-8;
    struct PBtn { const char* lbl; Color c; };
    PBtn pbts[] = {{"Tutup",{80,80,90,255}},{"Gadai",{180,80,60,255}},{"Bangun",{60,140,80,255}}};
    for (int i = 0; i < 3; i++) {
        Rectangle b = {px+6+i*(btnW+4),btnY,btnW,36};
        DrawRectangleRec(b,pbts[i].c);
        DrawRectangleLinesEx(b,1,{255,255,255,60});
        int tw = MeasureText(pbts[i].lbl,12);
        DrawText(pbts[i].lbl,(int)(b.x+btnW/2-tw/2),(int)(btnY+11),12,WHITE);
        if (i==0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(),b)) {
            showPopup = false; selectedTile = -1;
        }
    }
}