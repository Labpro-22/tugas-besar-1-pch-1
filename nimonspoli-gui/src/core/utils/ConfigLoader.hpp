#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

struct PropertyData {
    int    id;
    std::string code, name, type, color;
    double price, mortgage, upgHouse, upgHotel;
    std::vector<double> rentLevels; // index 0=L0, 1=L1, ... 5=L5
};

struct TaxConfig {
    double pphFlat       = 150;
    double pphPercentage = 10;
    double pbmFlat       = 200;
};

struct SpecialConfig {
    int goSalary = 200;
    int jailFine = 50;
};

struct MiscConfig {
    int maxTurn       = 50;
    int initialBalance = 1000;
};
class ConfigLoader {
public:
    explicit ConfigLoader(std::string basePath);

    std::vector<PropertyData>   loadProperties();   // property.txt
    std::map<int, int>          loadRailroad();      // railroad.txt
    std::map<int, int>          loadUtility();       // utility.txt
    TaxConfig                   loadTax();           // tax.txt
    SpecialConfig               loadSpecial();       // special.txt
    MiscConfig                  loadMisc();          // misc.txt

private:
    std::string basePath;
    std::ifstream openFile(const std::string& filename);

    std::vector<std::string> parseLine(const std::string& line);
};