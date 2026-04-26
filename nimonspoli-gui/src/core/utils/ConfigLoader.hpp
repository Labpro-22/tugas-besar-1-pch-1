#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

class PropertyData {
public:
    int    id;
    std::string code, name, type, color;
    double price, mortgage, upgHouse, upgHotel;
    std::vector<double> rentLevels; // index 0=L0, 1=L1, ... 5=L5
};

class ActionData {
public:
    int id;
    std::string code, name, type, color;
};

class TaxConfig {
public:
    double pphFlat       = 150;
    double pphPercentage = 10;
    double pbmFlat       = 200;
};

class SpecialConfig {
public:
    int goSalary = 200;
    int jailFine = 50;
};

class MiscConfig {
public:
    int maxTurn        = 50;
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
    std::vector<ActionData>     loadActions();

private:
    std::string basePath;
    std::ifstream openFile(const std::string& filename);
    std::vector<std::string> parseLine(const std::string& line);
};