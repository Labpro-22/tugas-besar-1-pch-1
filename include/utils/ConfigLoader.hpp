#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "../src/core/Property/Property.hpp"
#include "../src/core/Property/StreetProperty.hpp"
#include "../src/core/Property/RailroadProperty.hpp"
#include "../src/core/Property/UtilityProperty.hpp"

/** * Struct untuk menampung data mentah dari file konfigurasi.
 * Struct ini digunakan agar Data Access Layer tetap terpisah dari Core Logic.
 */

struct PropertyData
{
    int id;
    std::string code, name, type, color;
    double price, mortgage, upgHouse, upgHotel;
    std::vector<double> rentLevels; // Rent L0 sampai L5 [cite: 1774]
};

struct TaxConfig
{
    double pphFlat;
    double pphPercentage;
    double pbmFlat;
};

struct SpecialConfig
{
    int goSalary;
    int jailFine;
};

struct MiscConfig
{
    int maxTurn;
    int initialBalance;
};

class ConfigLoader
{
private:
    std::string basePath; // Menyimpan direktori tempat file config [cite: 175]

    // Method pembantu internal untuk parsing teks [cite: 180]
    std::vector<std::string> parseLine(const std::string &line);

    // Membuka file dan melempar exception jika gagal [cite: 181]
    std::ifstream openFile(const std::string &filename);

public:
    // Konstruktor menerima path folder config [cite: 175]
    ConfigLoader(std::string path);

    // Method untuk memuat data dari masing-masing file konfigurasi
    std::vector<PropertyData> loadProperties(); // Membaca property.txt [cite: 177]
    std::map<int, int> loadRailroad();          // Membaca railroad.txt [cite: 178]
    std::map<int, int> loadUtility();           // Membaca utility.txt [cite: 179]
    TaxConfig loadTax();                        // Membaca tax.txt [cite: 179]
    SpecialConfig loadSpecial();                // Membaca special.txt [cite: 179]
    MiscConfig loadMisc();                      // Membaca misc.txt [cite: 179]
};

#endif