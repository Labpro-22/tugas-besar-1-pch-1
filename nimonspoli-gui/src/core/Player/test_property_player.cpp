// #include <iostream>
// #include <string>
// #include <map>

// #include "Player.hpp"
// #include "../Property/StreetProperty.hpp"
// #include "../Property/RailroadProperty.hpp"
// #include "../Property/UtilityProperty.hpp"

// using namespace std;

// static void printSection(const string &title)
// {
//     cout << "\n==================================================\n";
//     cout << title << "\n";
//     cout << "==================================================\n";
// }

// int main()
// {
//     // ==================================================
//     // TEST 1: Player tanpa properti
//     // ==================================================
//     Player p1("PemainKosong", 1500);

//     printSection("TEST 1 - PLAYER TANPA PROPERTI");
//     cout << p1.cetakProperti() << "\n";

//     // ==================================================
//     // TEST 2: Player dengan 1 StreetProperty
//     // ==================================================
//     Player p2("PemainStreet", 2000);

//     StreetProperty street(
//         1,
//         "JKT",
//         "JAKARTA",
//         "BIRU_TUA",
//         350,
//         300,
//         "PemainStreet",
//         200,
//         200,
//         {{0, 35},
//          {1, 175},
//          {2, 500},
//          {3, 1100},
//          {4, 1300},
//          {5, 1500}},
//         0,
//         false,
//         1,
//         0);

//     p2.addProperty(&street);

//     printSection("TEST 2 - PLAYER DENGAN 1 STREET PROPERTY");
//     cout << p2.cetakProperti() << "\n";

//     // ==================================================
//     // TEST 3: Player dengan properti campuran
//     // ==================================================
//     Player p3("PemainCampuran", 3000);

//     StreetProperty garut(
//         2,
//         "GRT",
//         "GARUT",
//         "COKLAT",
//         60,
//         40,
//         "PemainCampuran",
//         20,
//         50,
//         {{0, 2},
//          {1, 10},
//          {2, 30},
//          {3, 90},
//          {4, 160},
//          {5, 250}},
//         0,
//         false,
//         1,
//         0);

//     RailroadProperty gambir(
//         3,
//         "GBR",
//         "STASIUN_GAMBIR",
//         "DEFAULT",
//         200,
//         100,
//         "PemainCampuran",
//         {{1, 25},
//          {2, 50},
//          {3, 100},
//          {4, 200}});

//     UtilityProperty pln(
//         4,
//         "PLN",
//         "PLN",
//         "ABU_ABU",
//         150,
//         75,
//         "PemainCampuran",
//         {{1, 4},
//          {2, 10}});

//     p3.addProperty(&garut);
//     p3.addProperty(&gambir);
//     p3.addProperty(&pln);

//     printSection("TEST 3 - PLAYER DENGAN PROPERTI CAMPURAN");
//     cout << p3.cetakProperti() << "\n";

//     // ==================================================
//     // TEST 4: Player dengan property null
//     // ==================================================
//     Player p4("PemainNull", 1000);
//     p4.addProperty(nullptr);

//     printSection("TEST 4 - PLAYER DENGAN PROPERTY NULL");
//     cout << p4.cetakProperti() << "\n";

//     // ==================================================
//     // TEST 5: Hapus salah satu properti
//     // ==================================================
//     p3.removeProperty(&gambir);

//     printSection("TEST 5 - SETELAH HAPUS 1 PROPERTY");
//     cout << p3.cetakProperti() << "\n";

//     return 0;
// }