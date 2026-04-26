#include "SaveLoadManager.hpp"

// Include di .cpp untuk menghindari circular dependency
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"
#include "../Board/Board.hpp"

#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/PropertyFactory.hpp"
#include "../Property/UtilityProperty.hpp"

#include "../ComputerPlayer/ComputerPlayer.hpp"
#include "../Card/CardDeck.hpp"
#include "../Card/CardFactory.hpp"
#include "../Card/SkillCard.hpp"
#include "../Card/MoveCard.hpp"
#include "../Card/DemolitionCard.hpp"
#include "../Card/DiscountCard.hpp"
#include "../Card/LassoCard.hpp"
#include "../Card/ShieldCard.hpp"
#include "../Card/TeleportCard.hpp"

#include "TransactionLogger.hpp"
#include "../Exceptions/SaveLoadException.hpp"
#include <iostream>
#include <algorithm>

//  Format file (spek)
//  <TURN> <MAX_TURN>
//  <JUMLAH_PEMAIN>
//  <STATE_PEMAIN_1..N>
//  <URUTAN_GILIRAN>
//  <GILIRAN_AKTIF>
//  <STATE_PROPERTI>
//  <STATE_DECK>
//  <STATE_LOG>

// Utils

vector<string> SaveLoadManager::tokenize(const string &line)
{
    vector<string> tokens;
    istringstream ss(line);
    string tok;
    while (ss >> tok)
        tokens.push_back(tok);
    return tokens;
}

bool SaveLoadManager::fileExists(const string &filename) const
{
    ifstream f(filename);
    return f.good();
}

SaveLoadManager::SaveLoadManager() {}

// Save

void SaveLoadManager::save(const GameState &state, const string &filename)
{
    ofstream out(filename);
    if (!out.is_open())
    {
        throw SaveFileNotFoundException("SaveLoadManager: Tidak bisa membuka file " + filename);
    }

    // <TURN> <MAX_TURN>
    out << state.getCurrTurn() << " " << state.getMaxTurn() << "\n";

    // <JUMLAH_PEMAIN>
    auto players = state.getPlayers();
    out << players.size() << "\n";

    // <STATE_PEMAIN_1..N>
    savePlayers(out, state);

    // <URUTAN_GILIRAN>
    for (auto p : players)
    {
        out << p->getUsername() << " ";
    }

    out << "\n";

    // <GILIRAN_AKTIF>
    out << state.getCurrPlayer()->getUsername() << "\n";

    //  <STATE_PROPERTI>
    saveProperties(out, state);

    //  <STATE_DECK>
    saveDeck(out, state);

    //  <STATE_LOG>
    saveLogs(out, state);

    out.close();
}

// ─── savePlayers ─────────────────────────────────────────────────────────────
/*
<USERNAME> <UANG> <POSISI_PETAK> <STATUS> <HUMAN/COM> [DIFFICULTY]
<JUMLAH_KARTU_TANGAN>
<JENIS_KARTU_1> [NILAI] [DURASI]
*/
void SaveLoadManager::savePlayers(ofstream &out, const GameState &state)
{
    for (auto &p : state.getPlayers())
    {
        // Status string
        string statusStr;
        switch (p->getStatus())
        {
        case PlayerStatus::ACTIVE:
            statusStr = "ACTIVE";
            break;
        case PlayerStatus::JAILED:
            statusStr = "JAILED";
            break;
        case PlayerStatus::BANKRUPT:
            statusStr = "BANKRUPT";
            break;
        }

        // Kode petak posisi saat ini
        string tileCode = "GO";
        if (state.getBoard())
        {
            std::cout << "[DEBUG save] " << p->getUsername()
                      << " getPosition()=" << p->getPosition()
                      << " boardSize=" << state.getBoard()->getSize() << std::endl;
            Tile *t = state.getBoard()->getTile(p->getPosition());
            std::cout << "[DEBUG save] tile=" << (t ? t->getCode() : "null") << std::endl;
            if (t)
                tileCode = t->getCode();
        }

        // <USERNAME> <UANG> <POSISI_PETAK> <STATUS> <HUMAN/COM> [DIFFICULTY]
        out << p->getUsername() << " "
            << p->getBalance() << " "
            << tileCode << " "
            << statusStr;

        // Flag COM atau HUMAN
        ComputerPlayer *cp = dynamic_cast<ComputerPlayer *>(p);
        if (cp)
            out << " COM " << difficultyToString(cp->getDifficulty());
        else
            out << " HUMAN";

        out << "\n";

        // <JUMLAH_KARTU_TANGAN>
        auto &hand = p->getHand();
        out << hand.size() << "\n";

        // <JENIS_KARTU> [NILAI] [DURASI]
        for (auto *card : hand)
        {
            out << card->getType();
            if (auto *mc = dynamic_cast<MoveCard *>(card))
                out << " " << mc->getSteps();
            else if (auto *dc = dynamic_cast<DiscountCard *>(card))
                out << " " << dc->getDiscountPercent() << " " << dc->getDuration();
            out << "\n";
        }
    }
}

// SaveProperties
/*
<JUMLAH_PROPERTI>
<KODE_PETAK> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>
…
*/
void SaveLoadManager::saveProperties(ofstream &out, const GameState &state)
{
    if (!state.getBoard())
    {
        out << "0\n";
        return;
    }

    vector<PropertyTile *> propTiles; // <- Semua tile yang punya properti
    Board *board = state.getBoard();
    for (int i = 0; i < board->getSize(); i++)
    {
        auto *t = dynamic_cast<PropertyTile *>(board->getTile(i));
        if (t && t->getProperty())
            propTiles.push_back(t);
    }
    // <JUMLAH_PROPERTI>
    out << propTiles.size() << "\n";

    // <KODE_PETAK> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>
    for (auto *pt : propTiles)
    {
        Property *prop = pt->getProperty();

        // Status stirng
        string statusStr;
        switch (prop->getStatus())
        {
        case PropertyStatus::BANK:
            statusStr = "BANK";
            break;
        case PropertyStatus::MORTGAGED:
            statusStr = "MORTGAGED";
            break;
        case PropertyStatus::OWNED:
            statusStr = "OWNED";
            break;
        }

        // Pemilik
        string owner = prop->getOwnerId().empty() ? "BANK" : prop->getOwnerId();

        // Jenis Properti
        string type = "street";
        int fmult = 1, fdur = 0;
        string bldStr = "0";

        // <FMULT> <FDUR> <N_BANGUNAN>
        if (auto *sp = dynamic_cast<StreetProperty *>(prop))
        {
            type = "street";
            fmult = sp->getFestivalMultiplier();
            fdur = sp->getFestivalDuration();
            bldStr = sp->gethasHotel() ? "H" : to_string(sp->getBuildingCount());
        }
        else if (dynamic_cast<RailroadProperty *>(prop))
        {
            type = "railroad";
        }
        else if (dynamic_cast<UtilityProperty *>(prop))
        {
            type = "utiliy";
        }
        // <KODE_PETAK> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>
        out << prop->getCode() << " " << type << " " << owner << " " << statusStr << " " << fmult << " " << fdur << " " << bldStr << "\n";
    }
}

// SaveDeck
/*
<JUMLAH_KARTU_DECK_KEMAMPUAN>
<JENIS_KARTU_1>
…
*/
void SaveLoadManager::saveDeck(ofstream &out, const GameState &state)
{
    auto *skillDeck = state.getSkillDeck();
    if (!skillDeck)
    {
        out << "0\n0\n";
        return;
    }

    // Draw pile
    auto &drawPile = skillDeck->getDrawPile();
    out << drawPile.size() << "\n";
    for (auto *c : drawPile)
    {
        auto *sc = dynamic_cast<SkillCard *>(c);
        if (!sc)
            continue;

        out << sc->getType();
        if (auto *mc = dynamic_cast<MoveCard *>(sc))
        {
            out << " " << mc->getSteps();
        }
        else if (auto *dc = dynamic_cast<DiscountCard *>(sc))
        {
            out << " " << dc->getDiscountPercent() << " " << dc->getDuration();
        }
        out << "\n";
    }

    // Discard pile
    auto &discardPile = skillDeck->getDiscardPile();
    out << discardPile.size() << "\n";
    for (auto *c : discardPile)
    {
        auto *sc = dynamic_cast<SkillCard *>(c);
        if (!sc)
            continue;

        out << sc->getType();
        if (auto *mc = dynamic_cast<MoveCard *>(sc))
        {
            out << " " << mc->getSteps();
        }
        else if (auto *dc = dynamic_cast<DiscountCard *>(sc))
        {
            out << " " << dc->getDiscountPercent() << " " << dc->getDuration();
        }
        out << "\n";
    }
}

// SaveLogs
/*
<JUMLAH_ENTRI_LOG>
<TURN> <USERNAME> <JENIS_AKSI> <DETAIL>
…
*/
// <JUMLAH_ENTRI_LOG>
// <TURN> <USERNAME> <JENIS_AKSI> <DETAIL>
void SaveLoadManager::saveLogs(std::ofstream &out, const GameState &state)
{
    auto *logger = state.getLogger();
    if (!logger)
    {
        out << "0\n";
        return;
    }

    auto &logs = logger->getLogs();
    out << logs.size() << "\n";
    for (auto &e : logs)
    {
        out << e.turn << " "
            << e.username << " "
            << e.actionType << " "
            << e.detail << "\n";
    }
}

void SaveLoadManager::load(const std::string &filename, GameState &state)
{
    std::ifstream in(filename);
    if (!in.is_open())
        throw std::runtime_error("SaveLoadManager: File tidak ditemukan: " + filename);

    std::string line;

    // <TURN> <MAX_TURN>
    std::getline(in, line);
    std::cout << "[DEBUG load] baris 1: " << line << std::endl;

    auto tok = tokenize(line);
    if (tok.size() < 2)
        throw std::runtime_error("SaveLoadManager: Format file rusak (baris 1)");

    int savedTurn = std::stoi(tok[0]);
    int savedMaxTurn = std::stoi(tok[1]);

    // Restore turn ke GameState
    state.setCurrTurn(savedTurn);
    state.setMaxTurn(savedMaxTurn);

    // Baris 2: <JUMLAH_PEMAIN>
    std::getline(in, line);
    int playerCount = std::stoi(line);
    std::cout << "[DEBUG load] jumlah pemain: " << line << std::endl;

    // Load state tiap pemain
    loadPlayers(in, state);
    std::cout << "[DEBUG load] setelah loadPlayers" << std::endl;

    // Urutan giliran
    std::getline(in, line);
    // tok = username-username dalam urutan giliran
    // Untuk sekarang skip — urutan sudah ada di listPlayer
    std::cout << "[DEBUG load] urutan giliran: " << line << std::endl;

    // Giliran aktif
    std::getline(in, line);
    std::cout << "[DEBUG load] giliran aktif: " << line << std::endl;
    std::string activeUsername = tokenize(line)[0];
    // Cari index pemain aktif dan set ke state
    auto players = state.getPlayers();
    for (int i = 0; i < (int)players.size(); i++)
    {
        if (players[i]->getUsername() == activeUsername)
        {
            state.setCurrPlayerIdx(i);
            break;
        }
    }

    // Load properti
    loadProperties(in, state);
    std::cout << "[DEBUG load] setelah loadProperties" << std::endl;

    // Load deck
    loadDeck(in, state);
      std::cout << "[DEBUG load] setelah loadDeck" << std::endl;

    // Load log
    loadLogs(in, state);
    std::cout << "[DEBUG load] setelah loadLogs" << std::endl;

    in.close();
}

// loadPlayers
void SaveLoadManager::loadPlayers(std::ifstream &in, GameState &state)
{
    auto players = state.getPlayers();
    std::string line;

    for (auto *p : players)
    {
        // <USERNAME> <UANG> <POSISI_PETAK> <STATUS>
        getline(in, line);
        auto tok = tokenize(line);
        if (tok.empty())
            throw runtime_error("Format rusak (giliran aktif)");
        p->setUsername(tok[0]);
        if (tok.size() < 4)
            continue;

        // Balance — restore dengan operator+=
        int balance = stoi(tok[1]);
        int diff = balance - p->getBalance();
        if (diff > 0)
            *p += diff;
        else if (diff < 0)
            *p -= (-diff);

        // Posisi
        if (state.getBoard())
        {
            int idx = state.getBoard()->findTileIndexByCode(tok[2]);
            std::cout << "[DEBUG load] " << tok[0]
                      << " tileCode=" << tok[2]
                      << " idx=" << idx << std::endl;
            if (idx >= 0)
                p->setPosition(idx);
        }

        // Status
        if (tok[3] == "ACTIVE")
            p->setStatus(PlayerStatus::ACTIVE);
        else if (tok[3] == "JAILED")
            p->setStatus(PlayerStatus::JAILED);
        else if (tok[3] == "BANKRUPT")
            p->setStatus(PlayerStatus::BANKRUPT);

        // Jumlah kartu
        getline(in, line);
        int cardCount = stoi(line);

        // Recreate kartu dari type string pakai CardFactory
        // Clear tangan dulu supaya tidak duplikat
        for (int c = 0; c < (int)p->getHand().size(); c++)
        {
            SkillCard *removed = p->discardSkillCard(0);
            delete removed;
        }

        for (int c = 0; c < cardCount; c++)
        {
            getline(in, line);
            auto ctok = tokenize(line);
            if (ctok.empty())
                continue;

            string type = ctok[0];

            double value = ctok.size() > 1 ? stod(ctok[1]) : 0;
            int duration = ctok.size() > 2 ? stoi(ctok[2]) : 0;

            SkillCard *card = CardFactory::createSkillCard(type, value, duration);
            if (card)
                p->addSkillCard(card);
        }
    }
}

// loadProperties
void SaveLoadManager::loadProperties(std::ifstream &in, GameState &state)
{
    std::string line;
    std::getline(in, line);
    int count = std::stoi(line);

    Board *board = state.getBoard();
    if (!board)
    {
        // Skip semua baris properti
        for (int i = 0; i < count; i++)
            std::getline(in, line);
        return;
    }

    // Clear properti semua pemain dulu supaya sinkron
    for (auto *p : state.getPlayers())
    {
        if (p)
            p->clearProperties();
    }

    for (int i = 0; i < count; i++)
    {
        std::getline(in, line);
        auto tok = tokenize(line);
        // Format: <KODE> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>
        if (tok.size() < 7)
            continue;

        std::string code = tok[0];
        std::string type = tok[1]; // street/railroad/utility
        std::string owner = tok[2];
        std::string statusStr = tok[3];
        int fmult = std::stoi(tok[4]);
        int fdur = std::stoi(tok[5]);
        std::string bldStr = tok[6];

        // Cari tile di board
        Tile *tile = board->findTileByCode(code);
        auto *pt = dynamic_cast<PropertyTile *>(tile);
        if (!pt || !pt->getProperty())
            continue;

        Property *prop = pt->getProperty();

        // Restore status
        if (statusStr == "BANK")
            prop->setStatus(PropertyStatus::BANK);
        else if (statusStr == "OWNED")
            prop->setStatus(PropertyStatus::OWNED);
        else if (statusStr == "MORTGAGED")
            prop->setStatus(PropertyStatus::MORTGAGED);

        // Restore owner
        if (owner == "BANK")
        {
            prop->clearOwner();
        }
        else
        {
            prop->setOwner(owner);
            // Sinkronisasi ke objek Player
            Player *p = state.getPlayerById(owner);
            if (p)
            {
                p->addProperty(prop);
            }
        }

        // Restore bangunan (hanya StreetProperty)
        if (auto *sp = dynamic_cast<StreetProperty *>(prop))
        {
            sp->setFestivalMultiplier(fmult);
            sp->setFestivalDuration(fdur);

            // Reset bangunan lalu rebuild sesuai save
            sp->resetBuildings(); // perlu ditambah di StreetProperty.hpp
            if (bldStr == "H")
            {
                // Hotel: bangun 4 rumah dulu lalu upgrade
                for (int b = 0; b < 4; b++)
                    sp->buildHouse();
                sp->upgToHotel();
            }
            else
            {
                int bldCount = std::stoi(bldStr);
                for (int b = 0; b < bldCount; b++)
                    sp->buildHouse();
            }
        }
    }
}

// ─── loadDeck ────────────────────────────────────────────────────────────────
// Baca urutan drawPile + discardPile, restore ke skill deck
void SaveLoadManager::loadDeck(std::ifstream &in, GameState &state)
{
    auto loadPile = [&](std::vector<SkillCard*>& pile) {
        int count;
        if (!(in >> count)) return;

        for (int i = 0; i < count; ++i)
        {
            std::string type;
            double value = 0;
            int duration = 0;

            in >> type;

            if (type == "MoveCard")
            {
                in >> value;
            }
            else if (type == "DiscountCard")
            {
                in >> value >> duration;
            }

            SkillCard *card = CardFactory::createSkillCard(type, value, duration);
            if (card)
            {
                pile.push_back(card);
            }
        }
    };

    std::vector<SkillCard *> newDrawPile;
    std::vector<SkillCard *> newDiscardPile;

    loadPile(newDrawPile);
    loadPile(newDiscardPile);

    CardDeck<SkillCard> *skillDeck = state.getSkillDeck();
    if (!skillDeck)
        return;

    skillDeck->setCardsNoShuffle(newDrawPile);
    skillDeck->setDiscardPileNoShuffle(newDiscardPile);

    // Consume trailing newline to not break next getline
    std::string dummy;
    std::getline(in, dummy);
}

// loadLogs
void SaveLoadManager::loadLogs(std::ifstream &in, GameState &state)
{
    std::string line;
    // Skip any leading whitespace (like the newline from loadDeck)
    in >> std::ws; 
    if (!std::getline(in, line) || line.empty()) return;

    int count = 0;
    try {
        count = std::stoi(line);
    } catch (...) {
        std::cout << "[ERROR loadLogs] Gagal parsing jumlah log dari: " << line << std::endl;
        return;
    }

    std::cout << "[DEBUG loadLogs] akan load " << count << " log" << std::endl;

    auto *logger = state.getLogger();
    if (logger)
        logger->clear();

    for (int i = 0; i < count; i++)
    {
        std::getline(in, line);
        auto tok = tokenize(line);
        if (tok.size() < 4)
            continue;

        int turn = std::stoi(tok[0]);
        std::string user = tok[1];
        std::string act = tok[2];
        // Detail adalah sisa token digabung
        std::string detail = "";
        for (int j = 3; j < (int)tok.size(); j++)
        {
            if (j > 3)
                detail += " ";
            detail += tok[j];
        }

        if (logger)
            logger->addLog(turn, user, act, detail);
    }
     std::cout << "[DEBUG loadLogs] setelah load: " 
              << (logger ? logger->getLogs().size() : 0) << " log" << std::endl;
}