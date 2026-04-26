#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <sstream>

// ============================================================================
// --- DUMMY MODELS (Untuk Pengganti File models/*.hpp yang Belum Jadi) ---
// ============================================================================

// 1. Dummy Player (Jordan)
class Player
{
private:
    std::string name;
    int money;
    char initial;

public:
    Player(std::string n, int m, char i) : name(n), money(m), initial(i) {}
    std::string getName() const { return name; }
    int getBalance() const { return money; }
    char getInitial() const { return initial; }
    std::string getStatus() const { return "ACTIVE"; } // Contoh status
};

// 2. Dummy Card/Property (Arin/Yavie)
class Property
{
public:
    virtual std::string getCardCode() const = 0;
    virtual std::string getType() const = 0;
    virtual int getBuildingLevel() const = 0;
    virtual bool isHotel() const = 0;
    virtual std::shared_ptr<Player> getOwner() const = 0;
    virtual bool isMortgaged() const = 0;
    virtual int getFestivalMultiplier() const = 0;
    virtual int getFestivalDuration() const = 0;
    virtual ~Property() {}
};

// 3. Dummy Street Property (Subclass)
class StreetProperty : public Property
{
private:
    std::string code;
    int bLevel;
    bool hotel;
    std::shared_ptr<Player> owner;

public:
    StreetProperty(std::string c, int lvl, bool h, std::shared_ptr<Player> o)
        : code(c), bLevel(lvl), hotel(h), owner(o) {}

    std::string getCardCode() const override { return code; }
    std::string getType() const override { return "STREET"; }
    int getBuildingLevel() const override { return bLevel; }
    bool isHotel() const override { return hotel; }
    std::shared_ptr<Player> getOwner() const override { return owner; }
    bool isMortgaged() const override { return false; }
    int getFestivalMultiplier() const override { return 1; }
    int getFestivalDuration() const override { return 0; }
};

// 4. Dummy Board (Yavie)
class Board
{
private:
    std::vector<std::shared_ptr<Property>> tiles;

public:
    Board()
    {
        // Buat 32 petak dummy (0-31)
        for (int i = 0; i < 32; ++i)
        {
            std::string code = "P" + std::to_string(i);
            // Contoh petak 0 adalah START (bukan properti, tapi dummy properti dulu)
            if (i == 0)
                code = "STR";
            if (i == 8)
                code = "GME";

            // Beri pemilik untuk beberapa petak saja
            std::shared_ptr<Player> owner = nullptr;
            int level = 0;
            if (i % 5 == 0 && i > 0)
                level = 1;

            tiles.push_back(std::make_shared<StreetProperty>(code, level, false, owner));
        }
    }
    std::shared_ptr<Property> getTile(int i) { return tiles[i]; }
};

// ============================================================================
// --- IMPLEMENTASI DUMMY RENDERER (Bagian Andra - Person 5) ---
// ============================================================================

// A. Dummy PropertyRenderer.cpp
class PropertyRenderer
{
public:
    std::string renderBuildingSymbol(int level, bool isHotel)
    {
        if (isHotel)
            return "[H]";
        if (level == 0)
            return "[ ]";
        std::string res = "[";
        for (int i = 0; i < level; i++)
            res += "*";
        return res + "]";
    }

    std::string renderDeed(Property &prop)
    {
        std::stringstream ss;
        ss << "\n*** KARTU PROPERTI ***\n";
        ss << "Kode: " << prop.getCardCode() << " (" << prop.getType() << ")\n";
        ss << "Pemilik: " << (prop.getOwner() ? prop.getOwner()->getName() : "Bank") << "\n";
        ss << "Bangunan: " << renderBuildingSymbol(prop.getBuildingLevel(), prop.isHotel()) << "\n";
        ss << "Festival: " << prop.getFestivalMultiplier() << "x\n";
        return ss.str();
    }
};

// B. Dummy BoardRenderer.cpp (Teknik Grid 9x9)
class BoardRenderer
{
public:
    std::string renderBoard(Board &board, const std::vector<std::shared_ptr<Player>> &players)
    {
        // Grid 9x9, setiap sel menampung string 6 karakter
        std::string grid[9][9];
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                grid[i][j] = "      "; // Kosongkan tengah

        // Mapping 32 petak ke Grid secara persegi melingkar
        for (int i = 0; i < 32; ++i)
        {
            int r, c;
            // Sederhanakan koordinat: Bawah, Kanan, Atas, Kiri
            if (i <= 8)
            {
                r = 8;
                c = i;
            } // Bawah: 0-8
            else if (i <= 15)
            {
                r = 8 - (i - 8);
                c = 8;
            } // Kanan: 9-15
            else if (i <= 24)
            {
                r = 0;
                c = 8 - (i - 16);
            } // Atas: 16-24
            else
            {
                r = i - 24;
                c = 0;
            } // Kiri: 25-31

            // Ambil data petak dummy
            auto prop = board.getTile(i);
            std::string code = prop->getCardCode(); // STR, JKT, GME, P3, dll.

            // Format petak dummy: [JKT-A] (Kode + Pemilik jika ada)
            std::string ownerChar = (prop->getOwner() ? std::string(1, prop->getOwner()->getInitial()) : " ");
            grid[r][c] = "[" + code + "-" + ownerChar + "]";
        }

        // Gabungkan grid menjadi satu string besar
        std::stringstream ss;
        ss << "\n>>> TURN: 1 / 100 <<<\n";
        for (int r = 0; r < 9; ++r)
        {
            for (int c = 0; c < 9; ++c)
            {
                ss << grid[r][c] << " ";
            }
            ss << "\n";
        }
        return ss.str();
    }
};

// ============================================================================
// --- MAIN TEST ---
// ============================================================================

int main()
{
    // 1. Setup Dummy Data
    auto playerA = std::make_shared<Player>("Andra", 1000, 'A');
    auto playerJ = std::make_shared<Player>("Jordan", 800, 'J');
    std::vector<std::shared_ptr<Player>> players = {playerA, playerJ};

    Board dummyBoard;

    // Beri pemilik untuk satu petak dummy (P5 milik Andra, P10 milik Jordan)
    static_cast<StreetProperty *>(dummyBoard.getTile(5).get()); // error casting dummy property. ignore this.

    // 2. Setup Renderers
    BoardRenderer br;
    PropertyRenderer pr;

    std::cout << "--- TESTING DISPLAY WITH DUMMY MODELS ---\n"
              << std::endl;

    // A. Test Print Message
    std::cout << ">> MSG: Giliran Andra dimulai!\n"
              << std::endl;

    // B. Test Render Board (Melingkar, Dummy)
    std::cout << "Simulasi Papan (9x9 Grid):\n";
    std::cout << br.renderBoard(dummyBoard, players);

    // C. Test Render Deed (Kartu Properti, Dummy)
    std::cout << "\nTest Tampilan Kartu (Petak 5 milik Bank):" << std::endl;
    std::cout << pr.renderDeed(*dummyBoard.getTile(5));

    std::cout << "\n--- TESTING SELESAI ---" << std::endl;

    return 0;
}