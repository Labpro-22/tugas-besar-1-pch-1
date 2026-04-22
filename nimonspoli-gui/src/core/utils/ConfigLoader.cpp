#include "ConfigLoader.hpp"
#include <iostream>
#include <stdexcept>


ConfigLoader::ConfigLoader(std::string path) : basePath(path) {}

std::ifstream ConfigLoader::openFile(const std::string& filename) {
    std::ifstream file(basePath + "/" + filename);
    if (!file.is_open()) {
        // Dilempar jika file tidak ditemukan
        throw std::runtime_error("ConfigException: File " + filename + " not found!");
    }
    return file;
}

std::vector<std::string> ConfigLoader::parseLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (tokenStream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<PropertyData> ConfigLoader::loadProperties() {
    auto file = openFile("property.txt"); // Membaca property.txt 
    std::vector<PropertyData> data;
    std::string line;

    // Lewati header file jika ada
    std::getline(file, line); 

    while (std::getline(file, line)) {
        auto tokens = parseLine(line);
        if (tokens.empty()) continue;

        PropertyData p;
        p.id = std::stoi(tokens[0]);
        p.code = tokens[1];
        p.name = tokens[2];
        p.type = tokens[3];
        p.color = tokens[4];
        p.price = std::stod(tokens[5]);
        p.mortgage = std::stod(tokens[6]);
        p.upgHouse = std::stod(tokens[7]);
        p.upgHotel = std::stod(tokens[8]);
        
        for (int i = 9; i < tokens.size(); ++i) {
            p.rentLevels.push_back(std::stod(tokens[i]));
        }
        data.push_back(p);
    }
    return data;
}

std::map<int, int> ConfigLoader::loadRailroad() {
    auto file = openFile("railroad.txt"); // Membaca railroad.txt
    std::map<int, int> rentMap;
    std::string line;

    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        auto tokens = parseLine(line);
        if (tokens.size() >= 2) {
            // tokens[0] = jumlah stasiun, tokens[1] = harga sewa
            rentMap[std::stoi(tokens[0])] = std::stoi(tokens[1]); 
        }
    }
    return rentMap;
}

// Lanjutan dari src/utils/ConfigLoader.cpp

std::map<int, int> ConfigLoader::loadUtility() {
    auto file = openFile("utility.txt"); // Membaca utility,txt
    std::map<int, int> factorMap;
    std::string line;

    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        auto tokens = parseLine(line);
        if (tokens.size() >= 2) {
            // tokens[0] = jumlah utility, tokens[1] = faktor pengali sewa 
            factorMap[std::stoi(tokens[0])] = std::stoi(tokens[1]); 
        }
    }
    return factorMap;
}

TaxConfig ConfigLoader::loadTax() {
    auto file = openFile("tax.txt");
    std::string line;
    
    std::getline(file, line); // Skip header 
    
    TaxConfig config;
    if (std::getline(file, line)) {
        auto tokens = parseLine(line);
        if (tokens.size() >= 3) {
            // tokens[0]=PPH Flat, tokens[1]=PPH %, tokens[2]=PBM Flat 
            config.pphFlat = std::stod(tokens[0]);
            config.pphPercentage = std::stod(tokens[1]);
            config.pbmFlat = std::stod(tokens[2]);
        }
    }
    return config;
}

SpecialConfig ConfigLoader::loadSpecial() {
    auto file = openFile("special.txt"); // Membaca special.txt
    std::string line;
    
    std::getline(file, line); // Skip header 
    
    SpecialConfig config;
    if (std::getline(file, line)) {
        auto tokens = parseLine(line);
        if (tokens.size() >= 2) {
            // tokens[0]=Gaji GO, tokens[1]=Denda Penjara 
            config.goSalary = std::stoi(tokens[0]);
            config.jailFine = std::stoi(tokens[1]);
        }
    }
    return config;
}

MiscConfig ConfigLoader::loadMisc() {
    auto file = openFile("misc.txt"); // Membaca misc.txt
    std::string line;
    
    std::getline(file, line); // Skip header
    MiscConfig config;
    if (std::getline(file, line)) {
        auto tokens = parseLine(line);
        if (tokens.size() >= 2) {
            // tokens[0]=Max Turn, tokens[1]=Saldo Awal [cite: 1829]
            config.maxTurn = std::stoi(tokens[0]);
            config.initialBalance = std::stoi(tokens[1]);
        }
    }
    return config;
}
