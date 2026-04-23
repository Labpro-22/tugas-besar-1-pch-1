#include "MainMenuScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <string>

static int SW() { return GetScreenWidth(); }
static int SH() { return GetScreenHeight(); }

namespace fs = std::filesystem;

MainMenuScreen::MainMenuScreen()
    : selectedPlayerCount(2), focusedInput(0),
      glowTimer(0.f), saveFileExists(false),
      readyToStart(false), errorTimer(0.f),
      showLoadPanel(false), selectedSaveIdx(-1),
      selectedDifficulty(COMDifficulty::MEDIUM)
{
    for (int i = 0; i < 4; i++)
    {
        nameBuffers[i] = "";
        isBot[i] = false;
    }
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::onEnter()
{
    readyToStart = false;
    errorMsg = "";
    showLoadPanel = false;
    selectedSaveIdx = -1;
    for (int i = 0; i < 4; i++)
        isBot[i] = false;
    selectedDifficulty = COMDifficulty::MEDIUM;
    scanSaveFiles();
}

void MainMenuScreen::onExit() {}

void MainMenuScreen::scanSaveFiles()
{
    saveFiles.clear();
    saveFileExists = false;
    if (!DirectoryExists("data"))
        return;
    try
    {
        for (auto &entry : fs::directory_iterator("data"))
        {
            if (!entry.is_regular_file())
                continue;
            if (entry.path().extension().string() == ".txt")
                saveFiles.push_back(entry.path().string());
        }
        std::sort(saveFiles.begin(), saveFiles.end());
    }
    catch (...)
    {
        if (FileExists("data/save.txt"))
            saveFiles.push_back("data/save.txt");
    }
    saveFileExists = !saveFiles.empty();
    if (selectedSaveIdx >= (int)saveFiles.size())
        selectedSaveIdx = saveFiles.empty() ? -1 : 0;
}

void MainMenuScreen::update(float dt)
{
    glowTimer += dt;
    if (errorTimer > 0)
        errorTimer -= dt;
    handleInput();
}

void MainMenuScreen::handleInput()
{
    if (showLoadPanel)
    {
        if (IsKeyPressed(KEY_ESCAPE))
            showLoadPanel = false;
        if (!saveFiles.empty())
        {
            if (IsKeyPressed(KEY_DOWN))
                selectedSaveIdx = (selectedSaveIdx + 1) % (int)saveFiles.size();
            if (IsKeyPressed(KEY_UP))
                selectedSaveIdx = (selectedSaveIdx + (int)saveFiles.size() - 1) % (int)saveFiles.size();
            if (IsKeyPressed(KEY_ENTER) && selectedSaveIdx >= 0)
                triggerLoad(saveFiles[selectedSaveIdx]);
        }
        return;
    }

    if (IsKeyPressed(KEY_TAB))
    {
        // Skip slot bot saat TAB
        do
        {
            focusedInput = (focusedInput + 1) % selectedPlayerCount;
        } while (isBot[focusedInput] && focusedInput < selectedPlayerCount);
    }

    if (focusedInput >= 0 && focusedInput < selectedPlayerCount && !isBot[focusedInput])
    {
        if (IsKeyPressed(KEY_BACKSPACE) && !nameBuffers[focusedInput].empty())
            nameBuffers[focusedInput].pop_back();

        int ch = GetCharPressed();
        while (ch > 0)
        {
            if (ch >= 32 && ch <= 126 && nameBuffers[focusedInput].size() < 16)
                nameBuffers[focusedInput] += (char)ch;
            ch = GetCharPressed();
        }

        if (IsKeyPressed(KEY_ENTER) && focusedInput < selectedPlayerCount - 1)
            focusedInput++;
    }
}

void MainMenuScreen::triggerLoad(const std::string &path)
{
    setup.isLoadGame = true;
    setup.saveFile = path;
    readyToStart = true;
    showLoadPanel = false;
}

bool MainMenuScreen::validateInputs()
{
    for (int i = 0; i < selectedPlayerCount; i++)
    {
        // Bot tidak perlu nama — auto-generate
        if (isBot[i] && nameBuffers[i].empty())
            nameBuffers[i] = "COM" + std::to_string(i + 1);

        if (nameBuffers[i].empty())
        {
            errorMsg = "Username player " + std::to_string(i + 1) + " tidak boleh kosong!";
            errorTimer = 3.f;
            return false;
        }
    }
    for (int i = 0; i < selectedPlayerCount; i++)
    {
        for (int j = i + 1; j < selectedPlayerCount; j++)
        {
            if (nameBuffers[i] == nameBuffers[j])
            {
                errorMsg = "Username tidak boleh sama!";
                errorTimer = 3.f;
                return false;
            }
        }
    }
    return true;
}

void MainMenuScreen::applySetup()
{
    setup.playerCount = selectedPlayerCount;
    setup.botDifficulty = selectedDifficulty;
    for (int i = 0; i < 4; i++)
    {
        setup.names[i] = (i < selectedPlayerCount) ? nameBuffers[i] : "";
        setup.isBot[i] = (i < selectedPlayerCount) ? isBot[i] : false;
    }
    setup.isLoadGame = false;
    setup.saveFile = "";
}

// ─── render ───────────────────────────────────────────────────────────────────
void MainMenuScreen::render(Window &window)
{
    (void)window;
    ClearBackground({14, 15, 31, 255});
    drawLeftPanel();
    drawRightPanel();
    drawTitle();
    drawError();
    if (showLoadPanel)
        drawLoadPanel();
}

// ─── drawTitle ────────────────────────────────────────────────────────────────
void MainMenuScreen::drawTitle()
{
    int screenW = SW();
    int screenH = SH();
    (void)screenH;

    const char *title = "NIMONSPOLI";
    int fontSize = std::min(130, screenW / 10);
    int tw = MeasureText(title, fontSize);
    int tx = screenW / 2 - tw / 2;
    int ty = 40;

    Color gc = {124, 111, 255, 0};
    int offsets[] = {10, 7, 4, 2};
    int alphas[] = {15, 35, 55, 85};
    for (int i = 0; i < 4; i++)
    {
        int o = offsets[i];
        gc.a = alphas[i];
        DrawText(title, tx - o, ty, fontSize, gc);
        DrawText(title, tx + o, ty, fontSize, gc);
        DrawText(title, tx, ty - o, fontSize, gc);
        DrawText(title, tx, ty + o, fontSize, gc);
        DrawText(title, tx - o, ty - o, fontSize, gc);
        DrawText(title, tx + o, ty - o, fontSize, gc);
        DrawText(title, tx - o, ty + o, fontSize, gc);
        DrawText(title, tx + o, ty + o, fontSize, gc);
    }

    float pulse = 0.85f + 0.15f * sinf(glowTimer * 2.f);
    DrawText(title, tx, ty, fontSize, {(unsigned char)(180 * pulse + 75), (unsigned char)(170 * pulse + 75), 255, 255});

    const char *sub = "BOARD GAME";
    int subSize = std::min(22, screenW / 55);
    int sw = MeasureText(sub, subSize);
    DrawText(sub, screenW / 2 - sw / 2, ty + fontSize + 8, subSize, {124, 111, 255, 200});
    DrawLine(0, ty + fontSize + 46, screenW, ty + fontSize + 46, {40, 42, 65, 255});
}

// ─── drawLeftPanel ────────────────────────────────────────────────────────────
void MainMenuScreen::drawLeftPanel()
{
    int screenW = SW();
    int screenH = SH();

    int titleBottom = 40 + std::min(130, screenW / 10) + 46;
    float px = 60.f;
    float pw = screenW / 2.f - 80.f;
    float top = (float)titleBottom + 10.f;

    DrawRectangle(0, titleBottom, screenW / 2, screenH - titleBottom, {16, 17, 28, 255});
    DrawLine(screenW / 2, titleBottom, screenW / 2, screenH, {50, 50, 80, 255});

    // ── Jumlah pemain ─────────────────────────────────────────────────────
    float sy = top + 20.f;
    DrawText("JUMLAH PEMAIN", (int)px, (int)sy, 14, {85, 85, 150, 255});
    sy += 26.f;

    float btnW = 110.f, btnH = 54.f, btnGap = 24.f;
    for (int i = 0; i < 3; i++)
    {
        int count = i + 2;
        float bx = px + i * (btnW + btnGap);
        bool sel = (selectedPlayerCount == count);
        float p = sel ? (0.7f + 0.3f * sinf(glowTimer * 3.f)) : 1.f;

        Rectangle btn = {bx, sy, btnW, btnH};
        DrawRectangleRec(btn, sel ? Color{35, 35, 75, 255} : Color{20, 21, 40, 255});
        DrawRectangleLinesEx(btn, sel ? 2.5f : 1.f,
                             sel ? Color{(unsigned char)(124 * p), (unsigned char)(111 * p), 255, 255}
                                 : Color{42, 45, 72, 255});

        std::string lbl = std::to_string(count);
        int lw = MeasureText(lbl.c_str(), 28);
        DrawText(lbl.c_str(), (int)(bx + btnW / 2 - lw / 2), (int)(sy + 12), 28,
                 sel ? WHITE : Color{85, 85, 120, 255});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), btn))
        {
            selectedPlayerCount = count;
            focusedInput = 0;
            for (int j = count; j < 4; j++)
                isBot[j] = false;
        }
    }
    sy += btnH + 16.f;

    // ── Difficulty COM ────────────────────────────────────────────────────
    DrawText("DIFFICULTY COM", (int)px, (int)sy, 14, {85, 85, 150, 255});
    sy += 20.f;

    const char *diffLabels[3] = {"EASY", "MEDIUM", "HARD"};
    COMDifficulty diffs[3] = {COMDifficulty::EASY, COMDifficulty::MEDIUM, COMDifficulty::HARD};

    for (int i = 0; i < 3; i++)
    {
        float bx = px + i * (btnW + btnGap);
        bool sel = (selectedDifficulty == diffs[i]);

        Rectangle btn = {bx, sy, btnW, 42};
        DrawRectangleRec(btn, sel ? Color{80, 60, 10, 255} : Color{20, 21, 40, 255});
        DrawRectangleLinesEx(btn, sel ? 2.f : 1.f,
                             sel ? Color{230, 170, 20, 255}
                                 : Color{42, 45, 72, 255});

        int lw = MeasureText(diffLabels[i], 12);
        DrawText(diffLabels[i], (int)(bx + btnW / 2 - lw / 2), (int)(sy + 14), 12,
                 sel ? Color{255, 220, 120, 255}
                     : Color{85, 85, 120, 255});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), btn))
        {
            selectedDifficulty = diffs[i];
        }
    }
    sy += 42.f + 24.f;

    // ── Input pemain ──────────────────────────────────────────────────────
    DrawText("PEMAIN", (int)px, (int)sy, 14, {85, 85, 150, 255});
    sy += 22.f;

    float totalH = screenH - sy - 28.f;
    float rowGap = 18.f;
    float rowH = (totalH - 3.f * rowGap) / 4.f;
    rowH = std::max(64.f, std::min(rowH, 92.f));

    for (int i = 0; i < 4; i++)
    {
        float iy = sy + i * (rowH + rowGap);
        bool active = i < selectedPlayerCount;

        Color dotCol = i == 0   ? Color{240, 60, 60, 255}
                       : i == 1 ? Color{240, 210, 60, 255}
                       : i == 2 ? Color{80, 180, 255, 255}
                                : Color{180, 100, 255, 255};

        DrawCircle((int)(px + 12), (int)(iy + rowH / 2), 12, active ? dotCol : Color{35, 37, 55, 255});

        float inputX = px + 32.f;
        float inputW = pw - 120.f;
        Rectangle inputBox = {inputX, iy, inputW, rowH};

        bool focused = (focusedInput == i && active && !isBot[i]);
        DrawRectangleRec(inputBox, active ? Color{20, 21, 40, 255} : Color{14, 15, 24, 255});
        DrawRectangleLinesEx(inputBox, focused ? 2.f : 1.f,
                             focused ? Color{124, 111, 255, 255}
                                     : Color{42, 45, 72, 255});

        std::string shown = nameBuffers[i];
        if (shown.empty())
            shown = "Player " + std::to_string(i + 1) + "...";

        DrawText(shown.c_str(),
                 (int)(inputX + 12),
                 (int)(iy + rowH / 2 - 10),
                 14,
                 active ? (nameBuffers[i].empty() ? Color{70, 74, 110, 255} : WHITE)
                        : Color{35, 37, 55, 255});

        if (active &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), inputBox))
        {
            focusedInput = i;
        }

        float toggleW = 82.f;
        float toggleX = inputX + inputW + 10.f;
        Rectangle toggleBtn = {toggleX, iy, toggleW, rowH};

        if (active)
        {
            bool bot = isBot[i];
            bool hov = CheckCollisionPointRec(GetMousePosition(), toggleBtn);

            Color togBg = hov ? Color{26, 28, 48, 255} : Color{20, 21, 40, 255};
            Color togBrd = bot ? Color{80, 200, 80, 255} : Color{124, 111, 255, 255};

            DrawRectangleRec(toggleBtn, togBg);
            DrawRectangleLinesEx(toggleBtn, 1.5f, togBrd);

            const char *typeLabel = bot ? "BOT" : "MAN";
            Color typeCol = bot ? Color{80, 200, 80, 255} : Color{150, 150, 220, 255};

            int tlw = MeasureText(typeLabel, 12);
            DrawText(typeLabel, (int)(toggleX + toggleW / 2 - tlw / 2),
                     (int)(iy + rowH / 2 - 8), 12, typeCol);

            DrawLine((int)(toggleX + 8), (int)(iy + rowH / 2 + 6),
                     (int)(toggleX + toggleW - 8), (int)(iy + rowH / 2 + 6), togBrd);

            const char *sub2 = bot ? "CPU" : "USR";
            int sw2 = MeasureText(sub2, 10);
            DrawText(sub2, (int)(toggleX + toggleW / 2 - sw2 / 2),
                     (int)(iy + rowH / 2 + 10), 10, {togBrd.r, togBrd.g, togBrd.b, 180});

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), toggleBtn))
            {
                isBot[i] = !isBot[i];
                if (isBot[i] && nameBuffers[i].empty())
                    nameBuffers[i] = "COM" + std::to_string(i + 1);
            }
        }
        else
        {
            DrawRectangleRec(toggleBtn, {16, 17, 26, 255});
            DrawRectangleLinesEx(toggleBtn, 1, {28, 30, 48, 255});
            int dlw = MeasureText("—", 14);
            DrawText("—", (int)(toggleX + toggleW / 2 - dlw / 2),
                     (int)(iy + rowH / 2 - 8), 14, {35, 37, 55, 255});
        }
    }
}
// ─── drawRightPanel ───────────────────────────────────────────────────────────
void MainMenuScreen::drawRightPanel()
{
    int titleBottom = 40 + 130 + 46;
    float rx = SCREEN_W / 2.f + 60;
    float rw = SCREEN_W / 2.f - 120;
    float top = (float)titleBottom + 10;

    DrawRectangle(SCREEN_W / 2, titleBottom, SCREEN_W / 2, SCREEN_H - titleBottom, {16, 17, 28, 255});

    float ry = top + 20;
    float totalH = SCREEN_H - ry - 40;

    float loadH = totalH * 0.20f;
    float gap1 = totalH * 0.04f;
    float startH = totalH * 0.55f;
    float gap2 = totalH * 0.03f;
    float exitH = totalH * 0.18f;

    // ── Load Game ─────────────────────────────────────────────────────────
    Rectangle loadBox = {rx, ry, rw, loadH};
    DrawRectangleRec(loadBox, {18, 20, 34, 255});
    DrawRectangleLinesEx(loadBox, 1, saveFileExists ? Color{60, 80, 140, 255} : Color{42, 45, 72, 255});
    DrawText("LOAD GAME", (int)(rx + 20), (int)(ry + 14), 14, {85, 85, 150, 255});

    if (saveFileExists)
    {
        std::string countStr = std::to_string(saveFiles.size()) + " file ditemukan";
        DrawText(countStr.c_str(), (int)(rx + 20), (int)(ry + 36), 13, {150, 150, 200, 255});

        if (selectedSaveIdx >= 0 && selectedSaveIdx < (int)saveFiles.size())
        {
            std::string preview = saveFiles[selectedSaveIdx];
            if (preview.substr(0, 5) == "data/")
                preview = preview.substr(5);
            DrawText(preview.c_str(), (int)(rx + 20), (int)(ry + 54), 11, {100, 120, 200, 255});
        }

        Rectangle lb = {rx + 20, ry + loadH - 42, rw - 40, 32};
        bool lh = CheckCollisionPointRec(GetMousePosition(), lb);
        float p = 0.7f + 0.3f * sinf(glowTimer * 2.5f);
        DrawRectangleRec(lb, lh ? Color{35, 50, 90, 255} : Color{22, 30, 60, 255});
        DrawRectangleLinesEx(lb, 1.5f,
                             Color{(unsigned char)(80 * p), (unsigned char)(100 * p), 200, 255});

        const char *loadLbl = "PILIH & LOAD SAVE";
        int lw2 = MeasureText(loadLbl, 13);
        DrawText(loadLbl, (int)(rx + 20 + (rw - 40) / 2 - lw2 / 2), (int)(ry + loadH - 28),
                 13, lh ? WHITE : Color{130, 150, 210, 255});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && lh)
        {
            scanSaveFiles();
            showLoadPanel = true;
            if (selectedSaveIdx < 0 && !saveFiles.empty())
                selectedSaveIdx = 0;
        }
    }
    else
    {
        DrawText("Tidak ada save file.", (int)(rx + 20), (int)(ry + 36), 14, {48, 51, 80, 255});
        DrawText("Mainkan game baru untuk membuat save.", (int)(rx + 20), (int)(ry + 56),
                 11, {38, 40, 65, 255});
    }

    // ── MULAI GAME ────────────────────────────────────────────────────────
    float startY = ry + loadH + gap1;
    Rectangle startBtn = {rx, startY, rw, startH};
    bool startHover = CheckCollisionPointRec(GetMousePosition(), startBtn);
    float pulse = 0.7f + 0.3f * sinf(glowTimer * 2.5f);
    Color sbc = {(unsigned char)(124 * pulse), (unsigned char)(111 * pulse), 255, 255};

    DrawRectangleRec(startBtn, startHover ? Color{28, 28, 58, 255} : Color{18, 19, 34, 255});
    DrawRectangleLinesEx(startBtn, 2.5f, sbc);

    const char *sl = "MULAI GAME";
    int slw = MeasureText(sl, 28);
    DrawText(sl, (int)(rx + rw / 2 - slw / 2), (int)(startY + startH / 2 - 20),
             28, startHover ? WHITE : Color{180, 170, 255, 255});

    const char *subLbl = "game baru";
    int subW = MeasureText(subLbl, 12);
    DrawText(subLbl, (int)(rx + rw / 2 - subW / 2), (int)(startY + startH / 2 + 10),
             12, {80, 80, 140, 255});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && startHover)
        if (validateInputs())
        {
            applySetup();
            readyToStart = true;
        }

    // ── KELUAR ────────────────────────────────────────────────────────────
    float exitY = startY + startH + gap2;
    Rectangle exitBtn = {rx, exitY, rw, exitH};
    bool exitHover = CheckCollisionPointRec(GetMousePosition(), exitBtn);

    DrawRectangleRec(exitBtn, exitHover ? Color{38, 12, 12, 255} : Color{18, 19, 34, 255});
    DrawRectangleLinesEx(exitBtn, 1.5f, {100, 30, 30, 255});

    const char *el = "KELUAR";
    int elw = MeasureText(el, 20);
    DrawText(el, (int)(rx + rw / 2 - elw / 2), (int)(exitY + exitH / 2 - 14),
             20, exitHover ? Color{255, 80, 80, 255} : Color{140, 50, 50, 255});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && exitHover)
        CloseWindow();
}

// ─── drawLoadPanel ────────────────────────────────────────────────────────────
void MainMenuScreen::drawLoadPanel()
{
    int screenW = SW();
    int screenH = SH();

    DrawRectangle(0, 0, screenW, screenH, {0, 0, 0, 180});

    constexpr float PW = 560.f;
    constexpr float PH = 420.f;
    float px = screenW / 2.f - PW / 2.f;
    float py = screenH / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {20, 22, 36, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 1.5f, {70, 80, 150, 255});

    DrawRectangle((int)px, (int)py, (int)PW, 48, {28, 30, 52, 255});
    DrawRectangleLinesEx({px, py, PW, 48}, 1, {70, 80, 150, 255});
    DrawText("PILIH SAVE FILE", (int)(px + 16), (int)(py + 16), 16, {180, 180, 240, 255});

    Rectangle refreshBtn = {px + PW - 100, py + 10, 60, 28};
    bool rHov = CheckCollisionPointRec(GetMousePosition(), refreshBtn);
    DrawRectangleRec(refreshBtn, rHov ? Color{50, 60, 100, 255} : Color{35, 40, 70, 255});
    DrawRectangleLinesEx(refreshBtn, 1, {80, 90, 160, 255});
    int rlw = MeasureText("REFRESH", 10);
    DrawText("REFRESH", (int)(refreshBtn.x + 30 - rlw / 2), (int)(refreshBtn.y + 9), 10, {150, 160, 220, 255});
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && rHov)
        scanSaveFiles();

    Rectangle xBtn = {px + PW - 36, py + 10, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);
    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
        showLoadPanel = false;

    if (saveFiles.empty())
    {
        DrawText("Tidak ada save file ditemukan.",
                 (int)(px + PW / 2 - MeasureText("Tidak ada save file ditemukan.", 14) / 2),
                 (int)(py + PH / 2 - 10), 14, {80, 80, 130, 255});
    }
    else
    {
        float listY = py + 56;
        float rowH = 48.f;
        float listH = PH - 56 - 72;

        BeginScissorMode((int)px, (int)listY, (int)PW, (int)listH);
        for (int i = 0; i < (int)saveFiles.size(); i++)
        {
            float ry = listY + i * rowH;
            if (ry > listY + listH)
                break;

            bool selected = (i == selectedSaveIdx);
            bool hover = CheckCollisionPointRec(GetMousePosition(), {px, ry, PW, rowH});

            Color rowBg = selected ? Color{35, 50, 90, 255}
                          : hover  ? Color{28, 35, 65, 255}
                                   : (i % 2 == 0 ? Color{22, 24, 38, 255} : Color{25, 27, 42, 255});
            DrawRectangle((int)px, (int)ry, (int)PW, (int)rowH - 1, rowBg);
            if (selected)
                DrawRectangle((int)px, (int)ry, 4, (int)rowH - 1, {100, 130, 255, 255});

            DrawText(std::to_string(i + 1).c_str(), (int)(px + 14), (int)(ry + 14), 14,
                     selected ? WHITE : Color{80, 80, 130, 255});

            std::string name = saveFiles[i];
            if (name.substr(0, 5) == "data/")
                name = name.substr(5);
            DrawText(name.c_str(), (int)(px + 40), (int)(ry + 12), 15,
                     selected ? WHITE : Color{160, 160, 210, 255});
            DrawText(saveFiles[i].c_str(), (int)(px + 40), (int)(ry + 28), 10, {80, 80, 120, 255});

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover)
                selectedSaveIdx = i;
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && hover && selected &&
                GetTime() - lastClickTime < 0.35)
                triggerLoad(saveFiles[i]);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover)
                lastClickTime = GetTime();
        }
        EndScissorMode();

        DrawLine((int)px, (int)(listY + listH), (int)(px + PW), (int)(listY + listH), {50, 55, 90, 255});

        float btnY = py + PH - 60;
        Rectangle loadBtn = {px + PW - 240, btnY, 110, 44};
        bool loadHov = CheckCollisionPointRec(GetMousePosition(), loadBtn);
        bool canLoad = (selectedSaveIdx >= 0 && selectedSaveIdx < (int)saveFiles.size());

        float lp = 0.7f + 0.3f * sinf(glowTimer * 3.f);
        DrawRectangleRec(loadBtn, canLoad ? (loadHov ? Color{40, 70, 160, 255} : Color{28, 48, 110, 255}) : Color{22, 25, 40, 255});
        DrawRectangleLinesEx(loadBtn, canLoad ? 2.f : 1.f,
                             canLoad ? Color{(unsigned char)(80 * lp), (unsigned char)(120 * lp), 255, 255} : Color{50, 55, 80, 255});
        int lbw = MeasureText("LOAD SAVE", 14);
        DrawText("LOAD SAVE", (int)(loadBtn.x + 55 - lbw / 2), (int)(loadBtn.y + 14), 14,
                 canLoad ? (loadHov ? WHITE : Color{160, 180, 255, 255}) : Color{60, 65, 100, 255});
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && loadHov && canLoad)
            triggerLoad(saveFiles[selectedSaveIdx]);

        Rectangle cancelBtn = {px + PW - 120, btnY, 96, 44};
        bool cancelHov = CheckCollisionPointRec(GetMousePosition(), cancelBtn);
        DrawRectangleRec(cancelBtn, cancelHov ? Color{50, 20, 20, 255} : Color{30, 15, 15, 255});
        DrawRectangleLinesEx(cancelBtn, 1, {100, 40, 40, 255});
        int cbw = MeasureText("BATAL", 14);
        DrawText("BATAL", (int)(cancelBtn.x + 48 - cbw / 2), (int)(cancelBtn.y + 14), 14,
                 cancelHov ? Color{255, 100, 100, 255} : Color{160, 70, 70, 255});
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && cancelHov)
            showLoadPanel = false;

        DrawText("↑↓ navigasi  ·  Enter untuk load  ·  Esc tutup",
                 (int)(px + 16), (int)(btnY + 16), 11, {60, 65, 100, 255});
    }
}

// ─── drawError ────────────────────────────────────────────────────────────────
void MainMenuScreen::drawError()
{
    if (errorTimer <= 0 || errorMsg.empty())
        return;
    float alpha = std::min(1.f, errorTimer);
    Color errC = {226, 75, 74, (unsigned char)(255 * alpha)};
    int ew = MeasureText(errorMsg.c_str(), 18);
    float ex = SCREEN_W / 2.f - ew / 2.f;
    float ey = SCREEN_H - 52;
    DrawRectangle((int)(ex - 16), (int)(ey - 10), ew + 32, 40, {40, 10, 10, (unsigned char)(200 * alpha)});
    DrawRectangleLinesEx({ex - 16, ey - 10, (float)(ew + 32), 40}, 1, errC);
    DrawText(errorMsg.c_str(), (int)ex, (int)ey, 18, errC);
}