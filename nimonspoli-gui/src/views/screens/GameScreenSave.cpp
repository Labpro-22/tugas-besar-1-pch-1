#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/utils/SaveLoadManager.hpp"
#include <filesystem>
#include <iostream>
#include <string>

void GameScreen::handleSimpan()
{
    savePopup.visible          = true;
    savePopup.confirmOverwrite = false;
    savePopup.resultVisible    = false;
    savePopup.fileNameInput    = "save";
}

void GameScreen::drawSavePopup()
{
    if (!savePopup.visible) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0,0,0,170});

    constexpr float PW = 480.f, PH = 260.f;
    float px = SCREEN_W/2.f - PW/2.f;
    float py = SCREEN_H/2.f - PH/2.f;

    DrawRectangle((int)px,(int)py,(int)PW,(int)PH,{22,24,38,255});
    DrawRectangleLinesEx({px,py,PW,PH},1.5f,{80,80,130,255});
    DrawRectangle((int)px,(int)py,(int)PW,44,{32,34,52,255});
    DrawRectangleLinesEx({px,py,PW,44},1,{80,80,130,255});
    DrawText("SIMPAN GAME",(int)(px+16),(int)(py+14),15,{180,180,230,255});

    // Tombol X
    Rectangle xBtn = {px+PW-36, py+8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(),xBtn);
    DrawRectangleRec(xBtn, xHov?Color{180,60,60,255}:Color{110,40,40,255});
    DrawText("X",(int)(xBtn.x+9),(int)(xBtn.y+8),12,WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov) {
        savePopup.visible = false;
        savePopup.confirmOverwrite = false;
    }

    if (!savePopup.confirmOverwrite) {
        // ── Mode input nama file ──────────────────────────────────────────
        DrawText("Nama file:",(int)(px+20),(int)(py+62),13,{150,150,200,255});
        DrawText("(folder: data/)",(int)(px+110),(int)(py+64),11,{80,80,120,255});

        Rectangle inputBox = {px+20, py+84, PW-100, 38};
        DrawRectangleRec(inputBox,{28,30,46,255});
        DrawRectangleLinesEx(inputBox,1.5f,{100,100,200,255});

        if (IsKeyPressed(KEY_BACKSPACE) && !savePopup.fileNameInput.empty())
            savePopup.fileNameInput.pop_back();
        int ch = GetCharPressed();
        while (ch > 0) {
            bool valid = (ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||
                         (ch>='0'&&ch<='9')||ch=='_'||ch=='-';
            if (valid && savePopup.fileNameInput.size() < 20)
                savePopup.fileNameInput += (char)ch;
            ch = GetCharPressed();
        }

        std::string disp = savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput;
        DrawText((disp+".txt").c_str(),(int)(inputBox.x+12),(int)(inputBox.y+11),
                 14, savePopup.fileNameInput.empty()?Color{60,60,100,255}:WHITE);
        if ((int)(GetTime()*2)%2==0 && !savePopup.fileNameInput.empty()) {
            int cw = MeasureText((savePopup.fileNameInput+".txt").c_str(),14);
            DrawText("|",(int)(inputBox.x+14+cw),(int)(inputBox.y+11),14,{120,120,255,255});
        }

        Rectangle okBtn = {px+PW-110, py+84, 86, 38};
        bool okHov = CheckCollisionPointRec(GetMousePosition(),okBtn);
        DrawRectangleRec(okBtn, okHov?Color{50,100,180,255}:Color{35,70,130,255});
        DrawRectangleLinesEx(okBtn,1,{80,130,220,255});
        int sw = MeasureText("SIMPAN",12);
        DrawText("SIMPAN",(int)(okBtn.x+43-sw/2),(int)(okBtn.y+13),12,WHITE);

        auto tryClick = [&]() {
            std::string fname = "data/" +
                (savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput) + ".txt";
            if (FileExists(fname.c_str())) savePopup.confirmOverwrite = true;
            else doSave(fname);
        };
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && okHov) tryClick();
        if (IsKeyPressed(KEY_ENTER)) tryClick();
        if (IsKeyPressed(KEY_ESCAPE)) { savePopup.visible = false; savePopup.confirmOverwrite = false; }

        if (savePopup.resultVisible && savePopup.resultTimer > 0) {
            float alpha = std::min(1.f, savePopup.resultTimer);
            Color rc = savePopup.resultOk
                     ? Color{80,200,120,(unsigned char)(255*alpha)}
                     : Color{220,80,80,(unsigned char)(255*alpha)};
            int rw = MeasureText(savePopup.resultMsg.c_str(),13);
            DrawText(savePopup.resultMsg.c_str(),
                     (int)(px+PW/2-rw/2),(int)(py+PH-44),13,rc);
        }

        DrawText("Enter untuk konfirmasi - Esc untuk tutup",
                 (int)(px+20),(int)(py+PH-24),11,{70,70,110,255});

    } else {
        // ── Mode konfirmasi timpa ─────────────────────────────────────────
        std::string fname = "data/" +
            (savePopup.fileNameInput.empty() ? "save" : savePopup.fileNameInput) + ".txt";
        std::string warn = "File \"" + fname + "\" sudah ada.";
        DrawText(warn.c_str(),(int)(px+20),(int)(py+70),13,{220,180,60,255});
        DrawText("Timpa file ini?",(int)(px+20),(int)(py+94),14,WHITE);

        Rectangle yaBtn = {px+60, py+136, 140, 44};
        bool yaHov = CheckCollisionPointRec(GetMousePosition(),yaBtn);
        DrawRectangleRec(yaBtn, yaHov?Color{60,180,90,255}:Color{40,120,60,255});
        DrawRectangleLinesEx(yaBtn,1,{80,220,120,255});
        int yw = MeasureText("YA, TIMPA",13);
        DrawText("YA, TIMPA",(int)(yaBtn.x+70-yw/2),(int)(yaBtn.y+15),13,WHITE);

        Rectangle noBtn = {px+PW-200, py+136, 140, 44};
        bool noHov = CheckCollisionPointRec(GetMousePosition(),noBtn);
        DrawRectangleRec(noBtn, noHov?Color{180,60,60,255}:Color{120,40,40,255});
        DrawRectangleLinesEx(noBtn,1,{220,80,80,255});
        int nw = MeasureText("TIDAK",13);
        DrawText("TIDAK",(int)(noBtn.x+70-nw/2),(int)(noBtn.y+15),13,WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (yaHov) { doSave(fname); savePopup.confirmOverwrite = false; }
            else if (noHov) savePopup.confirmOverwrite = false;
        }
        if (IsKeyPressed(KEY_ESCAPE)) savePopup.confirmOverwrite = false;
    }
}

void GameScreen::doSave(const std::string& filepath)
{
    bool ok = false;
    if (isRealMode() && guiManager) {
        GameMaster* gm = guiManager->getGameMaster();
        if (gm) {
            try {
                std::filesystem::create_directories("data");
                SaveLoadManager slm;
                slm.save(gm->getState(), filepath);
                gm->log("SYSTEM", "SIMPAN", "Game disimpan ke " + filepath);
                ok = true;
            } catch (const std::exception& e) {
                std::cerr << "[SIMPAN] Exception: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "[SIMPAN] Unknown exception!" << std::endl;
            }
        }
    } else {
        FILE* f = fopen(filepath.c_str(), "w");
        if (f) { fprintf(f, "MOCK_SAVE\n"); fclose(f); ok = true; }
    }

    if (ok) std::cout << "[SIMPAN] Berhasil ke: " << filepath << std::endl;
    else    std::cerr << "[SIMPAN] Gagal: "       << filepath << std::endl;

    savePopup.resultOk      = ok;
    savePopup.resultMsg     = ok ? ("Tersimpan: "+filepath) : "Gagal menyimpan!";
    savePopup.resultVisible = true;
    savePopup.resultTimer   = 3.f;
    if (ok) savePopup.visible = false;
}