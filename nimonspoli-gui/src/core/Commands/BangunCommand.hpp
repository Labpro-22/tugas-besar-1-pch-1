#pragma once

#include "./Command.hpp"
#include "../utils/TransactionLogger.hpp"
#include <string>
#include <vector>
#include <map>

class Player;
class StreetProperty;
class Bank;

class BangunCommand : public Command
{
public:
    // Kelas TileEntry untuk menggantikan struct
    class TileEntry
    {
    public:
        TileEntry(std::string code, std::string name, StreetProperty *prop)
            : code(code), name(name), prop(prop) {}

        std::string getCode() const { return code; }
        std::string getName() const { return name; }
        StreetProperty *getProp() const { return prop; }

    private:
        std::string code;
        std::string name;
        StreetProperty *prop;
    };

    // Constructor CLI: interaktif, prompt pengguna
    BangunCommand(Player *player, Bank *bank, TransactionLogger *logger, int turn);
    // Constructor GUI: langsung bangun/upgrade satu properti tertentu tanpa prompt
    BangunCommand(Player *player, StreetProperty *targetProp, bool isHotel,
                  TransactionLogger *logger, int turn);
    void execute(GameMaster &gm) override;

private:
    Player *player;
    Bank *bank;
    TransactionLogger *logger;
    int turn;
    // GUI-mode fields (nullptr = CLI mode)
    StreetProperty *guiTargetProp = nullptr;
    bool guiIsHotel = false;

    std::map<std::string, std::vector<TileEntry>> collectEligibleGroups(GameMaster &gm) const;
    bool canBuildOnTile(const TileEntry &entry, const std::vector<TileEntry> &group) const;
    bool allReadyForHotel(const std::vector<TileEntry> &group) const;
    void displayGroups(const std::map<std::string, std::vector<TileEntry>> &groups) const;
    void displayGroupDetail(const std::string &colorGroup, const std::vector<TileEntry> &group) const;
    void buildHouse(StreetProperty *prop, const std::string &propName, GameMaster &gm);
    void upgradeToHotel(StreetProperty *prop, const std::string &propName, GameMaster &gm);
};