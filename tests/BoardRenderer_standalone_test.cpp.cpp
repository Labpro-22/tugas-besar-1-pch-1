#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <map>

// ANSI colors
#define RST     "\033[0m"
#define C_CK    "\033[38;5;130m"
#define C_BM    "\033[38;5;117m"
#define C_PK    "\033[38;5;213m"
#define C_OR    "\033[38;5;214m"
#define C_MR    "\033[31m"
#define C_KN    "\033[33m"
#define C_HJ    "\033[32m"
#define C_BT    "\033[38;5;27m"
#define C_AB    "\033[38;5;245m"
#define C_DF    "\033[38;5;240m"

// ─────────────────────────────────────────────────────────────────
// LAYOUT TARGET (sesuai spesifikasi):
//   Sel        : "|" + 9 char konten = 10 char
//   Baris full : 11 × 10 + "|" = 111 char
//   Border full: 11 × "+----------" + "+" = 122 char
//   Baris tengah: kiri(10) + "|"(1) + MID(88) + "|"(1) + kanan(9) + "|"(1) = 110 char
//   Border tengah: 12 + spasi(86) + 12 = 110 char
// 
// Kita kecilkan MID dari 98 → 88 agar total baris tengah = 110
// sehingga tidak wrap di terminal standar 120-130 col
// ─────────────────────────────────────────────────────────────────
static const int MID_W = 88;
static const int MID_SP = 86; // spasi di border tengah: MID_W - 2

class Tile {public: std::string code, tag; const char* color; };
static Tile TILES[41] = {
/*0*/  {},
/*1*/  {"GO",  "[DF]", C_DF}, {"GRT","[CK]",C_CK}, {"DNU","[DF]",C_DF},
/*4*/  {"TSK", "[CK]", C_CK}, {"PPH","[DF]",C_DF}, {"GBR","[DF]",C_DF},
/*7*/  {"BGR", "[BM]", C_BM}, {"FES","[DF]",C_DF}, {"DPK","[BM]",C_BM},
/*10*/ {"BKS", "[BM]", C_BM}, {"PEN","[DF]",C_DF}, {"MGL","[PK]",C_PK},
/*13*/ {"PLN", "[AB]", C_AB}, {"SOL","[PK]",C_PK}, {"YOG","[PK]",C_PK},
/*16*/ {"STB", "[DF]", C_DF}, {"MAL","[OR]",C_OR}, {"DNU","[DF]",C_DF},
/*19*/ {"SMG", "[OR]", C_OR}, {"SBY","[OR]",C_OR}, {"BBP","[DF]",C_DF},
/*22*/ {"MKS", "[MR]", C_MR}, {"KSP","[DF]",C_DF}, {"BLP","[MR]",C_MR},
/*25*/ {"MND", "[MR]", C_MR}, {"TUG","[DF]",C_DF}, {"PLB","[KN]",C_KN},
/*28*/ {"PKB", "[KN]", C_KN}, {"PAM","[AB]",C_AB}, {"MED","[KN]",C_KN},
/*31*/ {"PPJ", "[DF]", C_DF}, {"BDG","[HJ]",C_HJ}, {"DEN","[HJ]",C_HJ},
/*34*/ {"FES", "[DF]", C_DF}, {"MTR","[HJ]",C_HJ}, {"GUB","[DF]",C_DF},
/*37*/ {"KSP", "[DF]", C_DF}, {"JKT","[BT]",C_BT}, {"PBM","[DF]",C_DF},
/*40*/ {"IKN", "[BT]", C_BT},
};

// ─────────────────────────────────────────────────────────────────
// State game (dummy untuk testing — ganti dengan data asli)
// Format info baris-2: "PX ^^^" atau "(2)" dll
// ─────────────────────────────────────────────────────────────────
class TileState {
    public:
    std::string owner;    // "" = tidak ada, "P1".."P4"
    int buildings = 0;    // 0-4 = rumah, 5 = hotel
    std::vector<int> players; // bidak pemain yang ada di sini
    bool mortgaged = false;
    bool inJail = false;
    int jailTurns = 0;
};

// Dummy state untuk demo
std::map<int, TileState> gameState;

void initDemoState() {
    // Contoh: P1 punya JKT dengan 1 rumah, ada bidak P4
    gameState[38] = {"P1", 1, {4}, false, false, 0};
    // P2 punya BDG dengan 2 rumah
    gameState[32] = {"P2", 2, {}, false, false, 0};
    // P3 ada di PEN (mampir)
    gameState[11] = {"", 0, {3}, false, false, 0};
    // P1 ada di GO
    gameState[1]  = {"", 0, {1}, false, false, 0};
    // Hotel di MKS
    gameState[22] = {"P2", 5, {}, false, false, 0};
    // Digadaikan
    gameState[33] = {"P3", 0, {}, true, false, 0};
}

// Build string info baris-2 untuk satu petak (maks 8 char)
std::string getTileInfo(int idx) {
    auto it = gameState.find(idx);
    if (it == gameState.end()) return "";

    auto& s = it->second;
    std::string info = "";

    // Bagian pemilik + bangunan
    if (!s.owner.empty()) {
        info += s.owner + " ";  // "P1 "
        if (s.mortgaged) {
            info += "[M]";
        } else if (s.buildings == 5) {
            info += "*";
        } else if (s.buildings > 0) {
            info += std::string(s.buildings, '^');
        }
    }

    // Bidak pemain
    if (!s.players.empty()) {
        if (!info.empty()) info += " ";
        for (int p : s.players) info += "(" + std::to_string(p) + ")";
    }

    // Potong ke 8 char
    if (info.size() > 8) info = info.substr(0, 8);
    return info;
}

// ─────────────────────────────────────────────────────────────────
// Cetak helpers
// ─────────────────────────────────────────────────────────────────

// Baris 1: "|[TAG] COD " = 10 char
void pTile1(int idx) {
    auto& t = TILES[idx];
    std::string c = t.code; c.resize(3, ' ');
    std::cout << "|" << t.color << t.tag << " " << c << RST << " ";
}

// Baris 1 kanan (dengan | penutup): "|[TAG] COD |"
void pTile1R(int idx) {
    auto& t = TILES[idx];
    std::string c = t.code; c.resize(3, ' ');
    std::cout << "|" << t.color << t.tag << " " << c << RST << " |";
}

// Baris 2: "|info     " = 10 char (info dinamis dari state)
void pTile2(int idx) {
    std::string info = getTileInfo(idx);
    info.resize(8, ' ');
    std::cout << "|" << info << " ";
}

// Baris 2 kanan
void pTile2R(int idx) {
    std::string info = getTileInfo(idx);
    info.resize(8, ' ');
    std::cout << "|" << info << " |";
}

void borderFull() {
    for (int i = 0; i < 11; i++) std::cout << "+----------";
    std::cout << "+" << std::endl;
}

void borderMid() {
    std::cout << "+----------+" << std::string(MID_SP, ' ') << "+----------+" << std::endl;
}

void pMid(const std::string& s) {
    std::ostringstream o;
    o << std::left << std::setw(MID_W) << s;
    std::cout << o.str();
}

void renderBoard(int turn, int maxTurn = 50) {
    std::vector<std::string> leg = {
        "  ==============================",
        "  ||      NIMONSPOLI         ||",
        "  ==============================",
        "          TURN " + std::to_string(turn) + " / " + std::to_string(maxTurn),
        "  ----------------------------",
        "  LEGENDA KEPEMILIKAN & STATUS",
        "  P1-P4: Properti milik Pemain",
        "  ^  : Rumah Lv1  ^^: Lv2",
        "  ^^^: Rumah Lv3  * : Hotel",
        "  (n): Bidak pemain n",
        "  [M]: Properti digadaikan",
        "  IN=Tahanan, V=Mampir",
        "  ----------------------------",
        "  KODE WARNA:",
        "  [CK]=Coklat  [MR]=Merah",
        "  [BM]=BiruMuda [KN]=Kuning",
        "  [PK]=Pink    [HJ]=Hijau",
        "  [OR]=Orange  [BT]=BiruTua",
        "  [DF]=Aksi    [AB]=Utilitas",
        ""
    };
    auto L = [&](int r) -> std::string {
        return r < (int)leg.size() ? leg[r] : "";
    };

    // ATAS: 21..31
    borderFull();
    for (int i = 21; i <= 31; i++) pTile1(i); std::cout << "|" << std::endl;
    for (int i = 21; i <= 31; i++) pTile2(i); std::cout << "|" << std::endl;
    borderFull();

    // TENGAH: kiri 20..12, kanan 32..40
    int lr = 0;
    for (int i = 0; i < 9; i++) {
        int lft = 20 - i, rgt = 32 + i;
        pTile1(lft); std::cout << "|"; pMid(L(lr++)); pTile1R(rgt); std::cout << std::endl;
        pTile2(lft); std::cout << "|"; pMid(L(lr++)); pTile2R(rgt); std::cout << std::endl;
        if (i < 8) borderMid();
    }

    // BAWAH: 11..1
    borderFull();
    for (int i = 11; i >= 1; i--) pTile1(i); std::cout << "|" << std::endl;
    for (int i = 11; i >= 1; i--) pTile2(i); std::cout << "|" << std::endl;
    borderFull();
}

int main() {
    initDemoState();
    std::cout << "\033[2J\033[H";
    std::cout << "--- TESTING DISPLAY NIMONSPOLI MILSTONE 1 ---\n";
    renderBoard(15, 50);

    std::cout << "\n[INFO] Sisi kanan mepet = simetris!\n";
    std::cout << "[INFO] Baris-2 sudah dinamis dari gameState.\n";
    std::cout << "[INFO] Contoh: JKT=P1 1rumah, BDG=P2 2rumah, MKS=P2 hotel, DEN=P3 gadai\n";
    return 0;
}