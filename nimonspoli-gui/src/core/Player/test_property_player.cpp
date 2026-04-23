#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"

using namespace std;

static bool contains(const string &text, const string &sub)
{
    return text.find(sub) != string::npos;
}

static void printDivider()
{
    cout << "============================================================\n";
}

static void printTestResult(const string &testName, bool passed)
{
    cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << "\n";
}

/*
 * Helper lokal untuk test:
 * Mencetak daftar properti milik player.
 *
 * Saya sengaja buat di file test supaya:
 * 1. Anda bisa langsung melihat output daftar property sekarang.
 * 2. Tidak tergantung apakah method Player::cetakProperti() sudah Anda implementasikan atau belum.
 *
 * Kalau nanti Anda sudah punya Player::cetakProperti(), fungsi ini bisa diganti menjadi:
 * return p.cetakProperti();
 */
static string cetakDaftarPropertiPlayer(const Player &p)
{
    ostringstream out;

    out << "============================================================\n";
    out << "DAFTAR PROPERTI MILIK PLAYER\n";
    out << "============================================================\n";
    out << "Username       : " << p.getUsername() << "\n";
    out << "Saldo          : " << p.getBalance() << "\n";
    out << "Jumlah Properti: " << p.getPropertyCount() << "\n\n";

    const vector<Property *> &props = p.getProperties();

    if (props.empty())
    {
        out << "Player ini tidak memiliki properti.\n";
        return out.str();
    }

    for (size_t i = 0; i < props.size(); ++i)
    {
        Property *prop = props[i];

        out << "---------------- PROPERTY #" << (i + 1) << " ----------------\n";

        if (prop == nullptr)
        {
            out << "Property null.\n\n";
            continue;
        }

        out << prop->cetakAkta();

        if (!out.str().empty() && out.str().back() != '\n')
        {
            out << '\n';
        }
        out << '\n';
    }

    return out.str();
}

int main()
{
    bool allPassed = true;

    // ============================================================
    // TEST 1: Player tanpa properti
    // ============================================================
    Player p1("PemainKosong", 1500);

    string out1 = cetakDaftarPropertiPlayer(p1);

    printDivider();
    cout << "TEST 1 - PLAYER TANPA PROPERTI\n";
    printDivider();
    cout << out1 << "\n";

    bool test1 =
        contains(out1, "PemainKosong") &&
        contains(out1, "Jumlah Properti: 0") &&
        contains(out1, "tidak memiliki properti");

    printTestResult("Player tanpa properti", test1);
    allPassed = allPassed && test1;

    // ============================================================
    // TEST 2: Player dengan 1 StreetProperty
    // ============================================================
    Player p2("PemainStreet", 2000);

    StreetProperty street1(
        1,
        "JKT",
        "Jakarta",
        "BIRU TUA",
        400,
        200,
        "",
        200,
        200,
        {{0, 50},
         {1, 200},
         {2, 600},
         {3, 1400},
         {4, 1700},
         {5, 2000}},
        0,
        false,
        1,
        0);

    street1.setOwner(p2.getUsername());
    p2.addProperty(&street1);

    string out2 = cetakDaftarPropertiPlayer(p2);

    printDivider();
    cout << "TEST 2 - PLAYER DENGAN 1 STREET PROPERTY\n";
    printDivider();
    cout << out2 << "\n";

    bool test2 =
        contains(out2, "PemainStreet") &&
        contains(out2, "Jumlah Properti: 1") &&
        contains(out2, "Jakarta") &&
        contains(out2, "JKT") &&
        contains(out2, "AKTA KEPEMILIKAN");

    printTestResult("Player dengan 1 street property", test2);
    allPassed = allPassed && test2;

    // ============================================================
    // TEST 3: Player dengan properti campuran
    // ============================================================
    Player p3("PemainCampuran", 3000);

    StreetProperty street2(
        2,
        "BDG",
        "Bandung",
        "BIRU MUDA",
        300,
        150,
        "",
        100,
        150,
        {{0, 30},
         {1, 90},
         {5, 700}},
        0,
        false,
        1,
        0);

    RailroadProperty rail1(
        3,
        "R1",
        "Stasiun Bandung",
        "RAILROAD",
        400,
        200,
        "",
        {{1, 25},
         {2, 50},
         {3, 100},
         {4, 200}});

    UtilityProperty util1(
        4,
        "U1",
        "PLN Pusat",
        "UTILITY",
        300,
        150,
        "",
        {{1, 4},
         {2, 10}});

    street2.setOwner(p3.getUsername());
    rail1.setOwner(p3.getUsername());
    util1.setOwner(p3.getUsername());

    p3.addProperty(&street2);
    p3.addProperty(&rail1);
    p3.addProperty(&util1);

    string out3 = cetakDaftarPropertiPlayer(p3);

    printDivider();
    cout << "TEST 3 - PLAYER DENGAN PROPERTI CAMPURAN\n";
    printDivider();
    cout << out3 << "\n";

    bool test3 =
        contains(out3, "PemainCampuran") &&
        contains(out3, "Jumlah Properti: 3") &&
        contains(out3, "Bandung") &&
        contains(out3, "Stasiun Bandung") &&
        contains(out3, "PLN Pusat") &&
        contains(out3, "BDG") &&
        contains(out3, "R1") &&
        contains(out3, "U1");

    printTestResult("Player dengan properti campuran", test3);
    allPassed = allPassed && test3;

    // ============================================================
    // TEST 4: Remove property
    // ============================================================
    p3.removeProperty(&rail1);
    string out4 = cetakDaftarPropertiPlayer(p3);

    printDivider();
    cout << "TEST 4 - REMOVE PROPERTY\n";
    printDivider();
    cout << out4 << "\n";

    bool test4 =
        contains(out4, "Jumlah Properti: 2") &&
        contains(out4, "Bandung") &&
        contains(out4, "PLN Pusat") &&
        !contains(out4, "Stasiun Bandung");

    printTestResult("Remove property", test4);
    allPassed = allPassed && test4;

    // ============================================================
    // TEST 5: Player dengan property null
    // ============================================================
    Player p4("PemainNull", 1000);
    p4.addProperty(nullptr);

    string out5 = cetakDaftarPropertiPlayer(p4);

    printDivider();
    cout << "TEST 5 - PLAYER DENGAN PROPERTY NULL\n";
    printDivider();
    cout << out5 << "\n";

    bool test5 =
        contains(out5, "PemainNull") &&
        contains(out5, "Jumlah Properti: 1") &&
        contains(out5, "Property null");

    printTestResult("Player dengan property null", test5);
    allPassed = allPassed && test5;

    // ============================================================
    // HASIL AKHIR
    // ============================================================
    printDivider();
    if (allPassed)
    {
        cout << "SEMUA TEST LULUS\n";
        return 0;
    }
    else
    {
        cout << "ADA TEST YANG GAGAL\n";
        return 1;
    }
}