// #include <iostream>
// #include <sstream>
// #include <string>
// #include <map>

// #include "CetakAktaCommand.hpp"
// #include "../Property/StreetProperty.hpp"
// #include "../Property/RailroadProperty.hpp"
// #include "../Property/UtilityProperty.hpp"

// // Dummy GameMaster untuk signature execute(GameMaster& gm)
// class GameMaster
// {
// };

// using namespace std;

// static string captureOutput(Command &cmd, GameMaster &gm)
// {
//     ostringstream oss;
//     streambuf *oldCout = cout.rdbuf(oss.rdbuf());

//     cmd.execute(gm);

//     cout.rdbuf(oldCout);
//     return oss.str();
// }

// static bool contains(const string &text, const string &sub)
// {
//     return text.find(sub) != string::npos;
// }

// static void printTestResult(const string &testName, bool passed)
// {
//     cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << "\n";
// }

// static void printCaptured(const string &title, const string &output)
// {
//     cout << "\n========== " << title << " ==========\n";
//     cout << output;
//     if (output.empty() || output.back() != '\n')
//     {
//         cout << '\n';
//     }
//     cout << "=====================================\n";
// }

// int main()
// {
//     GameMaster gm;
//     bool allPassed = true;

//     // ==================================================
//     // TEST 1: StreetProperty lengkap
//     // ==================================================
//     StreetProperty streetFull(
//         1,
//         "JKT",
//         "Jakarta",
//         "BIRU TUA",
//         400,
//         200,
//         "Pemain A",
//         200,
//         200,
//         {{0, 50},
//          {1, 200},
//          {2, 600},
//          {3, 1400},
//          {4, 1700},
//          {5, 2000}},
//         0,
//         false,
//         1,
//         0);

//     streetFull.setOwner("Pemain A");

//     CetakAktaCommand cmdStreetFull(&streetFull);
//     string outStreetFull = captureOutput(cmdStreetFull, gm);
//     printCaptured("STREET PROPERTY LENGKAP", outStreetFull);

//     bool testStreetFull =
//         contains(outStreetFull, "AKTA KEPEMILIKAN") &&
//         contains(outStreetFull, "Jakarta") &&
//         contains(outStreetFull, "JKT") &&
//         contains(outStreetFull, "Harga Beli") &&
//         contains(outStreetFull, "Nilai Gadai") &&
//         contains(outStreetFull, "Sewa (unimproved)") &&
//         contains(outStreetFull, "Sewa (1 rumah)") &&
//         contains(outStreetFull, "Sewa (2 rumah)") &&
//         contains(outStreetFull, "Sewa (3 rumah)") &&
//         contains(outStreetFull, "Sewa (4 rumah)") &&
//         contains(outStreetFull, "Sewa (hotel)") &&
//         contains(outStreetFull, "Harga Rumah") &&
//         contains(outStreetFull, "Harga Hotel") &&
//         contains(outStreetFull, "OWNED (Pemain A)");

//     printTestResult("StreetProperty lengkap", testStreetFull);
//     allPassed = allPassed && testStreetFull;

//     // ==================================================
//     // TEST 2: StreetProperty rent tidak lengkap
//     // ==================================================
//     StreetProperty streetPartial(
//         2,
//         "BDG",
//         "Bandung",
//         "BIRU MUDA",
//         300,
//         150,
//         "Pemain B",
//         100,
//         150,
//         {{0, 30},
//          {1, 90},
//          {5, 700}},
//         0,
//         false,
//         1,
//         0);

//     streetPartial.setOwner("Pemain B");

//     CetakAktaCommand cmdStreetPartial(&streetPartial);
//     string outStreetPartial = captureOutput(cmdStreetPartial, gm);
//     printCaptured("STREET PROPERTY RENT TIDAK LENGKAP", outStreetPartial);

//     bool testStreetPartial =
//         contains(outStreetPartial, "Bandung") &&
//         contains(outStreetPartial, "BDG") &&
//         contains(outStreetPartial, "Sewa (unimproved)") &&
//         contains(outStreetPartial, "Sewa (1 rumah)") &&
//         contains(outStreetPartial, "Sewa (hotel)") &&
//         !contains(outStreetPartial, "Sewa (2 rumah)") &&
//         !contains(outStreetPartial, "Sewa (3 rumah)") &&
//         !contains(outStreetPartial, "Sewa (4 rumah)") &&
//         contains(outStreetPartial, "OWNED (Pemain B)");

//     printTestResult("StreetProperty rent tidak lengkap", testStreetPartial);
//     allPassed = allPassed && testStreetPartial;

//     // ==================================================
//     // TEST 3: StreetProperty dengan key tidak biasa
//     // key 7 harus tampil sebagai level 7
//     // ==================================================
//     StreetProperty streetWeird(
//         3,
//         "SBY",
//         "Surabaya",
//         "HIJAU",
//         350,
//         175,
//         "Pemain C",
//         120,
//         170,
//         {{0, 40},
//          {2, 150},
//          {7, 999}},
//         0,
//         false,
//         1,
//         0);

//     streetWeird.setOwner("Pemain C");

//     CetakAktaCommand cmdStreetWeird(&streetWeird);
//     string outStreetWeird = captureOutput(cmdStreetWeird, gm);
//     printCaptured("STREET PROPERTY KEY TIDAK BIASA", outStreetWeird);

//     bool testStreetWeird =
//         contains(outStreetWeird, "Surabaya") &&
//         contains(outStreetWeird, "SBY") &&
//         contains(outStreetWeird, "Sewa (unimproved)") &&
//         contains(outStreetWeird, "Sewa (2 rumah)") &&
//         contains(outStreetWeird, "Sewa (level 7)") &&
//         contains(outStreetWeird, "OWNED (Pemain C)");

//     printTestResult("StreetProperty key tidak biasa", testStreetWeird);
//     allPassed = allPassed && testStreetWeird;

//     // ==================================================
//     // TEST 4: RailroadProperty lengkap
//     // ==================================================
//     RailroadProperty railroadFull(
//         4,
//         "R1",
//         "Stasiun Bandung",
//         "RAILROAD",
//         400,
//         200,
//         "Pemain D",
//         {{1, 25},
//          {2, 50},
//          {3, 100},
//          {4, 200}});

//     railroadFull.setOwner("Pemain D");

//     CetakAktaCommand cmdRailroadFull(&railroadFull);
//     string outRailroadFull = captureOutput(cmdRailroadFull, gm);
//     printCaptured("RAILROAD PROPERTY LENGKAP", outRailroadFull);

//     bool testRailroadFull =
//         contains(outRailroadFull, "Stasiun Bandung") &&
//         contains(outRailroadFull, "R1") &&
//         contains(outRailroadFull, "Sewa (1 railroad)") &&
//         contains(outRailroadFull, "Sewa (2 railroad)") &&
//         contains(outRailroadFull, "Sewa (3 railroad)") &&
//         contains(outRailroadFull, "Sewa (4 railroad)") &&
//         contains(outRailroadFull, "OWNED (Pemain D)");

//     printTestResult("RailroadProperty lengkap", testRailroadFull);
//     allPassed = allPassed && testRailroadFull;

//     // ==================================================
//     // TEST 5: RailroadProperty tidak lengkap
//     // ==================================================
//     RailroadProperty railroadPartial(
//         5,
//         "R2",
//         "Stasiun Kecil",
//         "RAILROAD",
//         250,
//         125,
//         "Pemain E",
//         {{1, 20},
//          {2, 40}});

//     railroadPartial.setOwner("Pemain E");

//     CetakAktaCommand cmdRailroadPartial(&railroadPartial);
//     string outRailroadPartial = captureOutput(cmdRailroadPartial, gm);
//     printCaptured("RAILROAD PROPERTY TIDAK LENGKAP", outRailroadPartial);

//     bool testRailroadPartial =
//         contains(outRailroadPartial, "Stasiun Kecil") &&
//         contains(outRailroadPartial, "R2") &&
//         contains(outRailroadPartial, "Sewa (1 railroad)") &&
//         contains(outRailroadPartial, "Sewa (2 railroad)") &&
//         !contains(outRailroadPartial, "Sewa (3 railroad)") &&
//         !contains(outRailroadPartial, "Sewa (4 railroad)") &&
//         contains(outRailroadPartial, "OWNED (Pemain E)");

//     printTestResult("RailroadProperty tidak lengkap", testRailroadPartial);
//     allPassed = allPassed && testRailroadPartial;

//     // ==================================================
//     // TEST 6: RailroadProperty key aneh
//     // ==================================================
//     RailroadProperty railroadWeird(
//         6,
//         "R3",
//         "Stasiun Aneh",
//         "RAILROAD",
//         260,
//         130,
//         "Pemain F",
//         {{2, 55},
//          {5, 500}});

//     railroadWeird.setOwner("Pemain F");

//     CetakAktaCommand cmdRailroadWeird(&railroadWeird);
//     string outRailroadWeird = captureOutput(cmdRailroadWeird, gm);
//     printCaptured("RAILROAD PROPERTY KEY ANEH", outRailroadWeird);

//     bool testRailroadWeird =
//         contains(outRailroadWeird, "Stasiun Aneh") &&
//         contains(outRailroadWeird, "R3") &&
//         contains(outRailroadWeird, "Sewa (2 railroad)") &&
//         contains(outRailroadWeird, "Sewa (5 railroad)") &&
//         contains(outRailroadWeird, "OWNED (Pemain F)");

//     printTestResult("RailroadProperty key aneh", testRailroadWeird);
//     allPassed = allPassed && testRailroadWeird;

//     // ==================================================
//     // TEST 7: UtilityProperty normal
//     // ==================================================
//     UtilityProperty utilityNormal(
//         7,
//         "U1",
//         "PLN Pusat",
//         "UTILITY",
//         300,
//         150,
//         "Pemain G",
//         {{1, 4},
//          {2, 10}});

//     utilityNormal.setOwner("Pemain G");

//     CetakAktaCommand cmdUtilityNormal(&utilityNormal);
//     string outUtilityNormal = captureOutput(cmdUtilityNormal, gm);
//     printCaptured("UTILITY PROPERTY NORMAL", outUtilityNormal);

//     bool testUtilityNormal =
//         contains(outUtilityNormal, "PLN Pusat") &&
//         contains(outUtilityNormal, "U1") &&
//         contains(outUtilityNormal, "Punya 1 utilitas: x4") &&
//         contains(outUtilityNormal, "Punya 2 utilitas: x10") &&
//         contains(outUtilityNormal, "OWNED (Pemain G)");

//     printTestResult("UtilityProperty normal", testUtilityNormal);
//     allPassed = allPassed && testUtilityNormal;

//     // ==================================================
//     // TEST 8: UtilityProperty variasi multiplier
//     // ==================================================
//     UtilityProperty utilityVariant(
//         8,
//         "U2",
//         "PDAM Kota",
//         "UTILITY",
//         280,
//         140,
//         "Pemain H",
//         {{1, 3},
//          {2, 8},
//          {3, 12}});

//     utilityVariant.setOwner("Pemain H");

//     CetakAktaCommand cmdUtilityVariant(&utilityVariant);
//     string outUtilityVariant = captureOutput(cmdUtilityVariant, gm);
//     printCaptured("UTILITY PROPERTY VARIASI MULTIPLIER", outUtilityVariant);

//     bool testUtilityVariant =
//         contains(outUtilityVariant, "PDAM Kota") &&
//         contains(outUtilityVariant, "U2") &&
//         contains(outUtilityVariant, "Punya 1 utilitas: x3") &&
//         contains(outUtilityVariant, "Punya 2 utilitas: x8") &&
//         contains(outUtilityVariant, "Punya 3 utilitas: x12") &&
//         contains(outUtilityVariant, "OWNED (Pemain H)");

//     printTestResult("UtilityProperty variasi multiplier", testUtilityVariant);
//     allPassed = allPassed && testUtilityVariant;

//     // ==================================================
//     // TEST 9: UtilityProperty kosong
//     // ==================================================
//     UtilityProperty utilityEmpty(
//         9,
//         "U3",
//         "Utility Kosong",
//         "UTILITY",
//         200,
//         100,
//         "Pemain I",
//         {});

//     utilityEmpty.setOwner("Pemain I");

//     CetakAktaCommand cmdUtilityEmpty(&utilityEmpty);
//     string outUtilityEmpty = captureOutput(cmdUtilityEmpty, gm);
//     printCaptured("UTILITY PROPERTY KOSONG", outUtilityEmpty);

//     bool testUtilityEmpty =
//         contains(outUtilityEmpty, "Utility Kosong") &&
//         contains(outUtilityEmpty, "U3") &&
//         contains(outUtilityEmpty, "Harga Beli") &&
//         contains(outUtilityEmpty, "Nilai Gadai") &&
//         contains(outUtilityEmpty, "OWNED (Pemain I)");

//     printTestResult("UtilityProperty kosong", testUtilityEmpty);
//     allPassed = allPassed && testUtilityEmpty;

//     // ==================================================
//     // TEST 10: Status BANK
//     // ==================================================
//     StreetProperty streetBank(
//         10,
//         "DPS",
//         "Denpasar",
//         "KUNING",
//         320,
//         160,
//         "",
//         100,
//         100,
//         {{0, 35},
//          {1, 90}},
//         0,
//         false,
//         1,
//         0);

//     CetakAktaCommand cmdStreetBank(&streetBank);
//     string outStreetBank = captureOutput(cmdStreetBank, gm);
//     printCaptured("STREET PROPERTY STATUS BANK", outStreetBank);

//     bool testStreetBank =
//         contains(outStreetBank, "Denpasar") &&
//         contains(outStreetBank, "DPS") &&
//         contains(outStreetBank, "Status : BANK");

//     printTestResult("StreetProperty status BANK", testStreetBank);
//     allPassed = allPassed && testStreetBank;

//     // ==================================================
//     // TEST 11: nullptr
//     // ==================================================
//     CetakAktaCommand cmdNull(nullptr);
//     string outNull = captureOutput(cmdNull, gm);
//     printCaptured("NULL PROPERTY", outNull);

//     bool testNull =
//         contains(outNull, "tidak ditemukan") ||
//         contains(outNull, "bukan properti") ||
//         contains(outNull, "tidak valid");

//     printTestResult("nullptr property", testNull);
//     allPassed = allPassed && testNull;

//     // ==================================================
//     // HASIL AKHIR
//     // ==================================================
//     cout << "\n==============================\n";
//     if (allPassed)
//     {
//         cout << "SEMUA TEST LULUS\n";
//         return 0;
//     }
//     else
//     {
//         cout << "ADA TEST YANG GAGAL\n";
//         return 1;
//     }
// }