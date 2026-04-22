#include "MainMenuScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include <cmath>
#include <algorithm>

MainMenuScreen::MainMenuScreen()
    : selectedPlayerCount(2), focusedInput(0),
      glowTimer(0.f), saveFileExists(false),
      readyToStart(false), errorTimer(0.f)
{
    nameBuffers[0] = "";
    nameBuffers[1] = "";
    nameBuffers[2] = "";
    nameBuffers[3] = "";
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::onEnter() {
    readyToStart = false;
    errorMsg     = "";
    checkSaveFile();
}

void MainMenuScreen::onExit() {}

void MainMenuScreen::checkSaveFile() {
    saveFileName   = "data/save.txt";
    saveFileExists = FileExists(saveFileName.c_str());
}

void MainMenuScreen::update(float dt) {
    glowTimer += dt;
    if (errorTimer > 0) errorTimer -= dt;
    handleInput();
}

void MainMenuScreen::handleInput() {
    if (IsKeyPressed(KEY_TAB))
        focusedInput = (focusedInput + 1) % selectedPlayerCount;

    if (focusedInput >= 0 && focusedInput < selectedPlayerCount) {
        if (IsKeyPressed(KEY_BACKSPACE) && !nameBuffers[focusedInput].empty())
            nameBuffers[focusedInput].pop_back();

        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch >= 32 && ch <= 126 && nameBuffers[focusedInput].size() < 16)
                nameBuffers[focusedInput] += (char)ch;
            ch = GetCharPressed();
        }

        if (IsKeyPressed(KEY_ENTER) && focusedInput < selectedPlayerCount - 1)
            focusedInput++;
    }
}

bool MainMenuScreen::validateInputs() {
    for (int i = 0; i < selectedPlayerCount; i++) {
        if (nameBuffers[i].empty()) {
            errorMsg   = "Username player " + std::to_string(i+1) + " tidak boleh kosong!";
            errorTimer = 3.f;
            return false;
        }
    }
    for (int i = 0; i < selectedPlayerCount; i++) {
        for (int j = i+1; j < selectedPlayerCount; j++) {
            if (nameBuffers[i] == nameBuffers[j]) {
                errorMsg   = "Username tidak boleh sama!";
                errorTimer = 3.f;
                return false;
            }
        }
    }
    return true;
}

void MainMenuScreen::applySetup() {
    setup.playerCount = selectedPlayerCount;
    for (int i = 0; i < 4; i++)
        setup.names[i] = (i < selectedPlayerCount) ? nameBuffers[i] : "";
    setup.isLoadGame = false;
    setup.saveFile   = "";
}

void MainMenuScreen::render(Window& window) {
    (void)window;
    ClearBackground({14, 15, 31, 255});
    drawLeftPanel();
    drawRightPanel();
    drawTitle();   // title di atas, draw terakhir supaya tidak ketimpa panel
    drawError();
}

// ─── drawTitle ────────────────────────────────────────────────────────────────
void MainMenuScreen::drawTitle() {
    const char* title = "NIMONSPOLI";
    int fontSize = 130;
    int tw = MeasureText(title, fontSize);
    int tx = SCREEN_W/2 - tw/2;
    int ty = 40;

    // Neon glow — 4 layer transparan di 8 arah
    Color gc = {124, 111, 255, 0};
    int offsets[] = {10, 7, 4, 2};
    int alphas[]  = {15, 35, 55, 85};
    for (int i = 0; i < 4; i++) {
        int o = offsets[i];
        gc.a = alphas[i];
        DrawText(title, tx-o, ty,   fontSize, gc);
        DrawText(title, tx+o, ty,   fontSize, gc);
        DrawText(title, tx,   ty-o, fontSize, gc);
        DrawText(title, tx,   ty+o, fontSize, gc);
        DrawText(title, tx-o, ty-o, fontSize, gc);
        DrawText(title, tx+o, ty-o, fontSize, gc);
        DrawText(title, tx-o, ty+o, fontSize, gc);
        DrawText(title, tx+o, ty+o, fontSize, gc);
    }

    float pulse = 0.85f + 0.15f * sinf(glowTimer * 2.f);
    DrawText(title, tx, ty, fontSize, {
        (unsigned char)(180*pulse+75),
        (unsigned char)(170*pulse+75),
        255, 255
    });

    const char* sub = "BOARD GAME";
    int sw = MeasureText(sub, 22);
    DrawText(sub, SCREEN_W/2-sw/2, ty+fontSize+8, 22, {124,111,255,200});

    // Garis pemisah title dengan panel
    DrawLine(0, ty+fontSize+46, SCREEN_W, ty+fontSize+46, {40,42,65,255});
}

// ─── drawLeftPanel ────────────────────────────────────────────────────────────
// Kiri: pilih jumlah pemain + input username
// Full height dari bawah title sampai bawah layar
void MainMenuScreen::drawLeftPanel() {
    int titleBottom = 40 + 130 + 46; // ty + fontSize + garis
    float px  = 60;
    float pw  = SCREEN_W/2.f - 80;
    float top = (float)titleBottom + 10;

    // Background kiri
    DrawRectangle(0, titleBottom, SCREEN_W/2, SCREEN_H - titleBottom,
                  {16, 17, 28, 255});
    // Garis tengah vertikal
    DrawLine(SCREEN_W/2, titleBottom, SCREEN_W/2, SCREEN_H, {50,50,80,255});

    // ── Jumlah pemain ─────────────────────────────────────────────────────
    float sy = top + 20;
    DrawText("JUMLAH PEMAIN", (int)px, (int)sy, 14, {85,85,150,255});
    sy += 26;

    float btnW = 110, btnH = 66, btnGap = 24;
    for (int i = 0; i < 3; i++) {
        int   count = i + 2;
        float bx    = px + i*(btnW+btnGap);
        bool  sel   = (selectedPlayerCount == count);
        float p     = sel ? (0.7f + 0.3f*sinf(glowTimer*3.f)) : 1.f;

        Rectangle btn = {bx, sy, btnW, btnH};
        DrawRectangleRec(btn, sel?Color{35,35,75,255}:Color{20,21,40,255});
        DrawRectangleLinesEx(btn, sel?2.5f:1.f,
            sel ? Color{(unsigned char)(124*p),(unsigned char)(111*p),255,255}
                : Color{42,45,72,255});

        std::string lbl = std::to_string(count);
        int lw = MeasureText(lbl.c_str(), 30);
        DrawText(lbl.c_str(), (int)(bx+btnW/2-lw/2), (int)(sy+16), 30,
                 sel?WHITE:Color{85,85,120,255});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), btn)) {
            selectedPlayerCount = count;
            focusedInput = 0;
        }
    }
    sy += btnH + 40;

    // ── Username inputs ───────────────────────────────────────────────────
    DrawText("USERNAME PEMAIN", (int)px, (int)sy, 14, {85,85,150,255});
    sy += 28;

    // Bagi sisa tinggi merata ke 4 input
    float bottomPad = 40;
    float totalH    = SCREEN_H - sy - bottomPad;
    float slotH     = totalH / 4.f;
    float inputH    = std::min(slotH * 0.72f, 80.f);
    float inputW    = pw - 52; // ruang untuk dot + label

    for (int i = 0; i < 4; i++) {
        float iy     = sy + i*slotH + (slotH - inputH)/2.f;
        bool  active  = (i < selectedPlayerCount);
        bool  focused = (focusedInput == i);

        // Dot warna player
        DrawCircle((int)(px+10), (int)(iy+inputH/2),
                   10, active?playerColors[i]:Color{28,30,48,255});

        float ix = px + 28;
        Rectangle rect = {ix, iy, inputW, inputH};

        Color bgC  = active ? Color{22,24,38,255} : Color{15,16,26,255};
        Color brdC = !active  ? Color{22,24,38,255}
                   : focused  ? Color{124,111,255,255}
                              : Color{48,51,80,255};

        DrawRectangleRec(rect, bgC);
        DrawRectangleLinesEx(rect, focused?2.f:1.f, brdC);

        int fs = 22;
        if (active) {
            std::string txt = nameBuffers[i];
            if (txt.empty() && !focused)
                DrawText(("Player "+std::to_string(i+1)+"...").c_str(),
                         (int)(ix+16),(int)(iy+inputH/2-fs/2), fs,
                         {45,48,76,255});
            else
                DrawText(txt.c_str(),(int)(ix+16),(int)(iy+inputH/2-fs/2),
                         fs, WHITE);

            if (focused && (int)(glowTimer*2)%2==0) {
                int cw = MeasureText(nameBuffers[i].c_str(), fs);
                DrawText("|",(int)(ix+16+cw),(int)(iy+inputH/2-fs/2),
                         fs, {124,111,255,255});
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), rect))
                focusedInput = i;
        } else {
            DrawText("—",(int)(ix+16),(int)(iy+inputH/2-11),
                     fs, {28,30,50,255});
        }
    }
}

// ─── drawRightPanel ───────────────────────────────────────────────────────────
// Kanan: load game + mulai + keluar — penuh tinggi
void MainMenuScreen::drawRightPanel() {
    int titleBottom = 40 + 130 + 46;
    float rx  = SCREEN_W/2.f + 60;
    float rw  = SCREEN_W/2.f - 120;
    float top = (float)titleBottom + 10;

    // Background kanan
    DrawRectangle(SCREEN_W/2, titleBottom, SCREEN_W/2, SCREEN_H-titleBottom,
                  {16,17,28,255});

    float ry        = top + 20;
    float totalH    = SCREEN_H - ry - 40;

    // Bagi tinggi: load 20%, mulai 55%, keluar 18%, gap 7%
    float loadH     = totalH * 0.20f;
    float gap1      = totalH * 0.04f;
    float startH    = totalH * 0.55f;
    float gap2      = totalH * 0.03f;
    float exitH     = totalH * 0.18f;

    // ── Load Game ─────────────────────────────────────────────────────────
    Rectangle loadBox = {rx, ry, rw, loadH};
    DrawRectangleRec(loadBox, {18,20,34,255});
    DrawRectangleLinesEx(loadBox, 1, {42,45,72,255});
    DrawText("LOAD GAME",(int)(rx+20),(int)(ry+14), 14, {85,85,150,255});

    if (saveFileExists) {
        DrawText(saveFileName.c_str(),(int)(rx+20),(int)(ry+36),14,{150,150,200,255});
        Rectangle lb = {rx+20, ry+loadH-44, rw-40, 34};
        bool lh = CheckCollisionPointRec(GetMousePosition(), lb);
        DrawRectangleRec(lb, lh?Color{30,35,60,255}:Color{20,22,40,255});
        DrawRectangleLinesEx(lb,1,{58,61,92,255});
        int lw2 = MeasureText("LOAD SAVE",14);
        DrawText("LOAD SAVE",(int)(rx+20+rw/2-lw2/2-20),(int)(ry+loadH-30),
                 14,{140,140,190,255});
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && lh) {
            setup.isLoadGame = true;
            setup.saveFile   = saveFileName;
            readyToStart     = true;
        }
    } else {
        DrawText("Tidak ada save file.",(int)(rx+20),(int)(ry+36),
                 14,{48,51,80,255});
    }

    // ── MULAI GAME ────────────────────────────────────────────────────────
    float startY    = ry + loadH + gap1;
    Rectangle startBtn = {rx, startY, rw, startH};
    bool startHover = CheckCollisionPointRec(GetMousePosition(), startBtn);
    float pulse     = 0.7f + 0.3f*sinf(glowTimer*2.5f);
    Color sbc       = {(unsigned char)(124*pulse),(unsigned char)(111*pulse),255,255};

    DrawRectangleRec(startBtn, startHover?Color{28,28,58,255}:Color{18,19,34,255});
    DrawRectangleLinesEx(startBtn, 2.5f, sbc);

    const char* sl = "MULAI GAME";
    int slw = MeasureText(sl, 28);
    DrawText(sl,(int)(rx+rw/2-slw/2),(int)(startY+startH/2-20),
             28, startHover?WHITE:Color{180,170,255,255});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && startHover)
        if (validateInputs()) { applySetup(); readyToStart = true; }

    // ── KELUAR ────────────────────────────────────────────────────────────
    float exitY     = startY + startH + gap2;
    Rectangle exitBtn = {rx, exitY, rw, exitH};
    bool exitHover  = CheckCollisionPointRec(GetMousePosition(), exitBtn);

    DrawRectangleRec(exitBtn, exitHover?Color{38,12,12,255}:Color{18,19,34,255});
    DrawRectangleLinesEx(exitBtn, 1.5f, {100,30,30,255});

    const char* el = "KELUAR";
    int elw = MeasureText(el, 20);
    DrawText(el,(int)(rx+rw/2-elw/2),(int)(exitY+exitH/2-14),
             20, exitHover?Color{255,80,80,255}:Color{140,50,50,255});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && exitHover)
        CloseWindow();
}

// ─── drawError ────────────────────────────────────────────────────────────────
void MainMenuScreen::drawError() {
    if (errorTimer <= 0 || errorMsg.empty()) return;
    float alpha = std::min(1.f, errorTimer);
    Color errC  = {226,75,74,(unsigned char)(255*alpha)};
    int   ew    = MeasureText(errorMsg.c_str(), 18);
    float ex    = SCREEN_W/2.f - ew/2.f;
    float ey    = SCREEN_H - 52;
    DrawRectangle((int)(ex-16),(int)(ey-10),ew+32,40,
                  {40,10,10,(unsigned char)(200*alpha)});
    DrawRectangleLinesEx({ex-16,ey-10,(float)(ew+32),40},1,errC);
    DrawText(errorMsg.c_str(),(int)ex,(int)ey,18,errC);
}