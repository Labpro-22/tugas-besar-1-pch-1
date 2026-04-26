#include "ComputerPlayer.hpp"

#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"
#include "../Board/Board.hpp"
#include "../Card/SkillCard.hpp"
#include "../Card/MoveCard.hpp"
#include "../Card/ShieldCard.hpp"
#include "../Card/TeleportCard.hpp"
#include "../Dice/Dice.hpp"
#include "../Bank/Bank.hpp"
#include "../Commands/BeliCommand.hpp"
#include "../Commands/FestivalCommand.hpp"
#include "../Card/LassoCard.hpp"
#include "../Card/TeleportCard.hpp"
#include "../Card/DemolitionCard.hpp"
#include "../Card/DiscountCard.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>
#include <sstream>

// ─────────────────────────────────────────────────────────────────
//  Utilitas global
// ─────────────────────────────────────────────────────────────────

static std::mt19937& getRNG() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

std::string difficultyToString(COMDifficulty diff) {
    switch (diff) {
        case COMDifficulty::EASY:   return "EASY";
        case COMDifficulty::MEDIUM: return "MEDIUM";
        case COMDifficulty::HARD:   return "HARD";
    }
    return "UNKNOWN";
}

// ─────────────────────────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────────────────────────

ComputerPlayer::ComputerPlayer(const std::string& username,
                               int startingBalance,
                               COMDifficulty diff)
    : Player(username, startingBalance),
      difficulty(diff)
{}

// ─────────────────────────────────────────────────────────────────
//  Getter
// ─────────────────────────────────────────────────────────────────

COMDifficulty ComputerPlayer::getDifficulty() const {
    return difficulty;
}

// ─────────────────────────────────────────────────────────────────
//  Utilitas internal
// ─────────────────────────────────────────────────────────────────

bool ComputerPlayer::randomChance(double probability) const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(getRNG()) < probability;
}

void ComputerPlayer::announceAction(const std::string& action) const {
    std::cout << "[COM - " << getUsername()
              << " (" << difficultyToString(difficulty) << ")] "
              << action << std::endl;
}

int ComputerPlayer::countSameColorOwned(const std::string& colorGroup,
                                         GameState& gs) const {
    int count = 0;
    for (Property* p : getProperties()) {
        if (p && p->getColorGroup() == colorGroup)
            ++count;
    }
    return count;
}

int ComputerPlayer::countSameColorTotal(const std::string& colorGroup,
                                         GameState& gs) const {
    Board* board = gs.getBoard();
    if (!board) return 0;
    int count = 0;
    for (int i = 0; i < board->getSize(); i++) {
        PropertyTile* pt = dynamic_cast<PropertyTile*>(board->getTile(i));
        if (!pt) continue;
        Property* prop = pt->getProperty();
        if (prop && prop->getColorGroup() == colorGroup)
            ++count;
    }
    return count;
}

int ComputerPlayer::estimateRentValue(Property* prop, GameState& gs) const {
    if (!prop) return 0;
    // Estimasi sewa dasar saat ini
    // Gunakan diceRoll = 7 (angka paling umum 2d6) untuk utility
    int rent = prop->calculateRentPrice(7,
        countSameColorOwned(prop->getColorGroup(), gs),
        false);
    return rent;
}

// Evaluasi skor properti untuk keputusan pembelian (HARD)
// Skor lebih tinggi = lebih menguntungkan dibeli
double ComputerPlayer::evaluatePropertyScore(Property* prop,
                                              GameState& gs) const {
    if (!prop) return 0.0;

    double score = 0.0;
    int price    = prop->getPurchasePrice();
    if (price <= 0) return 0.0;

    // Faktor 1: Efisiensi biaya-sewa (sewa / harga beli)
    int baseRent = estimateRentValue(prop, gs);
    score += (static_cast<double>(baseRent) / price) * 100.0;

    // Faktor 2: Potensi monopoli color group
    std::string color = prop->getColorGroup();
    int owned = countSameColorOwned(color, gs);
    int total = countSameColorTotal(color, gs);
    if (total > 0) {
        double monopolyProgress = static_cast<double>(owned + 1) / total;
        score += monopolyProgress * 50.0;

        // Bonus besar jika ini properti terakhir untuk monopoli
        if (owned + 1 == total)
            score += 80.0;
    }

    // Faktor 3: Sisa uang setelah beli (jangan sampai bangkrut)
    int remaining = getBalance() - price;
    if (remaining < 100)
        score -= 40.0; // penalti: terlalu miskin setelah beli
    else if (remaining > 500)
        score += 15.0; // bonus: finansial aman

    // Faktor 4: Posisi strategis (properti hijau/biru tua lebih mahal = sewa lebih tinggi)
    score += static_cast<double>(price) / 20.0;

    return score;
}

Property* ComputerPlayer::chooseBestFestivalProperty(GameState& gs) const {
    Property* best = nullptr;
    int bestRent   = -1;

    for (Property* p : getProperties()) {
        if (!p || p->getStatus() == PropertyStatus::MORTGAGED) continue;
        StreetProperty* sp = dynamic_cast<StreetProperty*>(p);
        if (!sp) continue;
        // Pilih yang sewa-nya paling tinggi dan festival-multiplier belum maksimal
        if (sp->getFestivalMultiplier() >= 8) continue;
        int rent = sp->calculateRentPrice(7,
            countSameColorOwned(sp->getColorGroup(), gs), false);
        if (rent > bestRent) {
            bestRent = rent;
            best     = p;
        }
    }
    return best;
}

Property* ComputerPlayer::chooseAnyFestivalProperty(GameState& gs) const {
    for (Property* p : getProperties()) {
        if (!p || p->getStatus() == PropertyStatus::MORTGAGED) continue;
        if (dynamic_cast<StreetProperty*>(p)) return p;
    }
    return nullptr;
}

Property* ComputerPlayer::chooseMortgageTarget(int needed, GameState& gs) const {
    // Gadaikan properti dengan nilai gadai tertinggi yang belum di-gadai
    // dan tidak punya bangunan (atau color group sudah dibersihkan)
    Property* target = nullptr;
    int       bestVal = -1;

    for (Property* p : getProperties()) {
        if (!p || p->getStatus() == PropertyStatus::MORTGAGED) continue;

        // Periksa apakah ada bangunan di color group
        bool hasBuildingsInGroup = false;
        for (Property* other : getProperties()) {
            if (!other || other->getColorGroup() != p->getColorGroup()) continue;
            StreetProperty* sp = dynamic_cast<StreetProperty*>(other);
            if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel())) {
                hasBuildingsInGroup = true;
                break;
            }
        }
        if (hasBuildingsInGroup) continue;

        int val = p->getMortageValue();
        if (val > bestVal) {
            bestVal = val;
            target  = p;
        }
    }
    return target;
}

Property* ComputerPlayer::chooseBuildTarget(GameState& gs) const {
    // Cari color group yang sudah dimonopoli lalu pilih petak dengan rumah paling sedikit
    GameMaster* gm = gs.getGameMaster();
    if (!gm) return nullptr;

    // Kumpulkan color group yang sudah dimonopoli
    std::vector<std::string> monopolyGroups;
    Board* board = gs.getBoard();
    if (!board) return nullptr;

    // Kumpulkan semua color group unik
    std::vector<std::string> allGroups;
    for (int i = 0; i < board->getSize(); i++) {
        PropertyTile* pt = dynamic_cast<PropertyTile*>(board->getTile(i));
        if (!pt) continue;
        Property* p = pt->getProperty();
        if (!p || dynamic_cast<RailroadProperty*>(p) || dynamic_cast<UtilityProperty*>(p))
            continue;
        std::string cg = p->getColorGroup();
        if (std::find(allGroups.begin(), allGroups.end(), cg) == allGroups.end())
            allGroups.push_back(cg);
    }

    for (const auto& cg : allGroups) {
        if (gm->hasMonopoly(const_cast<ComputerPlayer*>(this), cg))
            monopolyGroups.push_back(cg);
    }

    if (monopolyGroups.empty()) return nullptr;

    // Dari semua monopoly group, cari petak yang paling "layak dibangun"
    Property* target = nullptr;
    int minBuildings  = std::numeric_limits<int>::max();

    for (const auto& cg : monopolyGroups) {
        for (Property* p : getProperties()) {
            if (!p || p->getColorGroup() != cg) continue;
            if (p->getStatus() == PropertyStatus::MORTGAGED) continue;
            StreetProperty* sp = dynamic_cast<StreetProperty*>(p);
            if (!sp) continue;
            if (sp->gethasHotel()) continue; // sudah hotel

            int buildings = sp->getBuildingCount();
            if (buildings < minBuildings) {
                minBuildings = buildings;
                target       = p;
            }
        }
    }
    return target;
}

// ─────────────────────────────────────────────────────────────────
//  EASY: Keputusan Naif / Acak
//  - Beli properti: 50% peluang
//  - Bid lelang: selalu pass atau bid minimum
//  - Tidak pernah bangun
//  - Tidak pernah gadai kecuali terpaksa
//  - Pilih pajak flat selalu
//  - Tidak pernah pakai skill card
// ─────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────
//  MEDIUM: Keputusan Berbasis Aturan
//  - Beli properti: jika cukup uang dan properti tidak terlalu mahal
//  - Bid lelang: bid sampai 70% saldo atau batas yang masuk akal
//  - Bangun jika punya monopoli dan ada uang cadangan
//  - Gadai properti dengan sewa terendah saat perlu uang
//  - Pilih pajak yang lebih hemat
//  - Pakai shield card jika dalam bahaya
// ─────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────
//  HARD: Keputusan Strategis (Heuristic)
//  - Beli properti: evaluasi skor strategis
//  - Bid lelang: bid agresif untuk properti bernilai tinggi
//  - Bangun sesegera mungkin setelah monopoli
//  - Gadai properti sewa terendah, tebus yang menguntungkan
//  - Pilih pajak berdasarkan kalkulasi aktual kekayaan
//  - Pilih festival untuk properti sewa tertinggi
//  - Gunakan skill card secara optimal
// ─────────────────────────────────────────────────────────────────

bool ComputerPlayer::decideAndBuy(Property* prop, GameState& gs) {
    if (!prop) return false;
    int price = prop->getPurchasePrice();

    switch (difficulty) {
        // ── EASY ─────────────────────────────────────────────────────
        case COMDifficulty::EASY: {
            bool buy = randomChance(0.5) && canAfford(price);
            if (buy)
                announceAction("Membeli " + prop->getName() + " seharga M" + std::to_string(price) + " (keputusan acak).");
            else
                announceAction("Melewati pembelian " + prop->getName() + " (keputusan acak).");
            return buy;
        }

        // ── MEDIUM ───────────────────────────────────────────────────
        case COMDifficulty::MEDIUM: {
            if (!canAfford(price)) {
                announceAction("Tidak mampu membeli " + prop->getName() + ".");
                return false;
            }
            // Beli jika: harga ≤ 60% saldo, ATAU properti ini melengkapi monopoli
            int    owned = countSameColorOwned(prop->getColorGroup(), gs);
            int    total = countSameColorTotal(prop->getColorGroup(), gs);
            bool   nearMonopoly = (total > 0 && owned + 1 == total);
            bool   affordable   = (price <= getBalance() * 0.60);

            if (affordable || nearMonopoly) {
                announceAction("Membeli " + prop->getName() +
                    (nearMonopoly ? " (melengkapi monopoli!)" : " (harga wajar)."));
                return true;
            }
            announceAction("Melewati " + prop->getName() + " — terlalu mahal relatif terhadap saldo.");
            return false;
        }

        // ── HARD ─────────────────────────────────────────────────────
        case COMDifficulty::HARD: {
            if (!canAfford(price)) {
                announceAction("Tidak mampu membeli " + prop->getName() + ".");
                return false;
            }
            double score = evaluatePropertyScore(prop, gs);
            // Threshold: beli jika skor > 40 (cukup menguntungkan)
            if (score > 40.0) {
                std::ostringstream oss;
                oss << "Membeli " << prop->getName()
                    << " (skor strategi: " << static_cast<int>(score) << ").";
                announceAction(oss.str());
                return true;
            }
            announceAction("Melewati " + prop->getName() +
                " — skor strategi rendah (" + std::to_string(static_cast<int>(score)) + ").");
            return false;
        }
    }
    return false;
}

int ComputerPlayer::decideBid(Property* prop, int currentBid, GameState& gs) {
    if (!prop) return 0;

    switch (difficulty) {
        // ── EASY ─────────────────────────────────────────────────────
        case COMDifficulty::EASY: {
            // 30% chance ikut bid dengan nilai acak (currentBid + 10..50)
            if (!randomChance(0.30) || getBalance() <= currentBid) {
                announceAction("PASS pada lelang " + prop->getName() + ".");
                return 0;
            }
            std::uniform_int_distribution<int> dist(10, 50);
            int bid = currentBid + dist(getRNG());
            if (bid > getBalance()) bid = getBalance();
            announceAction("BID M" + std::to_string(bid) + " pada lelang " + prop->getName() + " (acak).");
            return bid;
        }

        // ── MEDIUM ───────────────────────────────────────────────────
        case COMDifficulty::MEDIUM: {
            int price    = prop->getPurchasePrice();
            int maxBid   = static_cast<int>(getBalance() * 0.50); // berani bid max 50% saldo
            maxBid       = std::min(maxBid, price);               // tidak mau melebihi harga beli
            if (currentBid >= maxBid) {
                announceAction("PASS pada lelang " + prop->getName() + " — sudah terlalu tinggi.");
                return 0;
            }
            int bid = currentBid + 10;
            bid = std::min(bid, maxBid);
            announceAction("BID M" + std::to_string(bid) + " pada lelang " + prop->getName() + ".");
            return bid;
        }

        // ── HARD ─────────────────────────────────────────────────────
        case COMDifficulty::HARD: {
            double score  = evaluatePropertyScore(prop, gs);
            int    price  = prop->getPurchasePrice();

            // Hitung nilai wajar maksimum yang mau dibayar berdasarkan skor
            double willingness = 0.0;
            if (score > 120.0)      willingness = 1.20; // mau bayar melebihi harga beli
            else if (score > 80.0)  willingness = 1.00;
            else if (score > 50.0)  willingness = 0.75;
            else if (score > 30.0)  willingness = 0.50;
            else {
                announceAction("PASS pada lelang " + prop->getName() + " — skor terlalu rendah.");
                return 0;
            }

            int maxWilling = static_cast<int>(price * willingness);
            maxWilling     = std::min(maxWilling, getBalance());

            if (currentBid >= maxWilling) {
                announceAction("PASS pada lelang " + prop->getName() +
                    " — harga sudah melebihi batas kesediaan (M" + std::to_string(maxWilling) + ").");
                return 0;
            }

            // Bid naik bertahap
            int step = std::max(10, (maxWilling - currentBid) / 3);
            int bid  = currentBid + step;
            bid      = std::min(bid, maxWilling);

            announceAction("BID M" + std::to_string(bid) + " pada lelang " + prop->getName() +
                " (skor: " + std::to_string(static_cast<int>(score)) + ").");
            return bid;
        }
    }
    return 0;
}

Property* ComputerPlayer::decideMortgage(int amountNeeded, GameState& gs) {
    switch (difficulty) {
        // ── EASY ─────────────────────────────────────────────────────
        case COMDifficulty::EASY: {
            // Gadaikan properti pertama yang bisa digadaikan
            for (Property* p : getProperties()) {
                if (!p || p->getStatus() == PropertyStatus::MORTGAGED) continue;
                bool hasBldg = false;
                for (Property* other : getProperties()) {
                    if (!other || other->getColorGroup() != p->getColorGroup()) continue;
                    StreetProperty* sp = dynamic_cast<StreetProperty*>(other);
                    if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel())) {
                        hasBldg = true; break;
                    }
                }
                if (hasBldg) continue;
                announceAction("Menggadaikan " + p->getName() + " (pilihan pertama yang tersedia).");
                return p;
            }
            return nullptr;
        }

        // ── MEDIUM ───────────────────────────────────────────────────
        case COMDifficulty::MEDIUM: {
            // Gadaikan yang nilai gadainya cukup untuk menutupi kebutuhan
            for (Property* p : getProperties()) {
                if (!p || p->getStatus() == PropertyStatus::MORTGAGED) continue;
                bool hasBldg = false;
                for (Property* other : getProperties()) {
                    if (!other || other->getColorGroup() != p->getColorGroup()) continue;
                    StreetProperty* sp = dynamic_cast<StreetProperty*>(other);
                    if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel())) {
                        hasBldg = true; break;
                    }
                }
                if (hasBldg) continue;
                if (p->getMortageValue() >= amountNeeded) {
                    announceAction("Menggadaikan " + p->getName() + " (nilai gadai cukup untuk menutupi kebutuhan).");
                    return p;
                }
            }
            // Fallback: nilai terbesar
            return chooseMortgageTarget(amountNeeded, gs);
        }

        // ── HARD ─────────────────────────────────────────────────────
        case COMDifficulty::HARD: {
            // Gadaikan properti dengan sewa terendah (pengorbanan paling sedikit)
            Property* target   = nullptr;
            int       lowestRent = std::numeric_limits<int>::max();

            for (Property* p : getProperties()) {
                if (!p || p->getStatus() == PropertyStatus::MORTGAGED) continue;
                bool hasBldg = false;
                for (Property* other : getProperties()) {
                    if (!other || other->getColorGroup() != p->getColorGroup()) continue;
                    StreetProperty* sp = dynamic_cast<StreetProperty*>(other);
                    if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel())) {
                        hasBldg = true; break;
                    }
                }
                if (hasBldg) continue;

                // Hindari menggadaikan properti yang sedang mendekati monopoli
                int owned = countSameColorOwned(p->getColorGroup(), gs);
                int total = countSameColorTotal(p->getColorGroup(), gs);
                if (total > 0 && owned == total) continue; // sudah monopoli → jangan gadai

                int rent = estimateRentValue(p, gs);
                if (rent < lowestRent) {
                    lowestRent = rent;
                    target     = p;
                }
            }
            if (target)
                announceAction("Menggadaikan " + target->getName() + " (sewa terendah — pengorbanan minimal).");
            return target;
        }
    }
    return nullptr;
}

bool ComputerPlayer::decideRedeem(Property* prop, GameState& gs) {
    if (!prop || prop->getStatus() != PropertyStatus::MORTGAGED) return false;
    int redeemCost = prop->getPurchasePrice(); // tebus = harga beli penuh

    switch (difficulty) {
        case COMDifficulty::EASY:
            // Tidak pernah tebus secara proaktif
            return false;

        case COMDifficulty::MEDIUM: {
            // Tebus jika: cukup uang DAN properti punya potensi monopoli
            if (!canAfford(redeemCost + 200)) return false; // sisakan 200 cadangan
            int owned = countSameColorOwned(prop->getColorGroup(), gs);
            int total = countSameColorTotal(prop->getColorGroup(), gs);
            bool nearMonopoly = (total > 0 && owned >= total - 1);
            if (nearMonopoly) {
                announceAction("Menebus " + prop->getName() + " (mendekati monopoli).");
                return true;
            }
            return false;
        }

        case COMDifficulty::HARD: {
            if (!canAfford(redeemCost + 100)) return false;
            double score = evaluatePropertyScore(prop, gs);
            if (score > 60.0) {
                announceAction("Menebus " + prop->getName() +
                    " (skor strategi tinggi: " + std::to_string(static_cast<int>(score)) + ").");
                return true;
            }
            return false;
        }
    }
    return false;
}

std::pair<std::string, std::string> ComputerPlayer::decideBuild(GameState& gs) {
    switch (difficulty) {
        case COMDifficulty::EASY:
            // Tidak pernah membangun
            return {"", ""};

        case COMDifficulty::MEDIUM: {
            // Bangun jika punya monopoli dan saldo > 500
            if (getBalance() < 500) return {"", ""};
            Property* target = chooseBuildTarget(gs);
            if (!target) return {"", ""};
            StreetProperty* sp = dynamic_cast<StreetProperty*>(target);
            if (!sp) return {"", ""};
            int cost = sp->gethasHotel() ? sp->getHotelUpgCost() : sp->getHouseUpgCost();
            if (!canAfford(cost + 200)) return {"", ""};
            announceAction("Membangun di " + target->getName() + ".");
            return {target->getColorGroup(), target->getCode()};
        }

        case COMDifficulty::HARD: {
            // Bangun sesegera mungkin setelah punya monopoli
            if (getBalance() < 150) return {"", ""};
            Property* target = chooseBuildTarget(gs);
            if (!target) return {"", ""};
            StreetProperty* sp = dynamic_cast<StreetProperty*>(target);
            if (!sp) return {"", ""};
            int cost = sp->gethasHotel() ? sp->getHotelUpgCost() : sp->getHouseUpgCost();
            if (!canAfford(cost)) return {"", ""};
            announceAction("Membangun di " + target->getName() + " (strategi agresif).");
            return {target->getColorGroup(), target->getCode()};
        }
    }
    return {"", ""};
}

std::string ComputerPlayer::decideFestivalProperty(GameState& gs) {
    switch (difficulty) {
        case COMDifficulty::EASY: {
            // Pilih properti pertama yang tersedia
            for (Property* p : getProperties()) {
                if (p && p->getStatus() != PropertyStatus::MORTGAGED
                    && dynamic_cast<StreetProperty*>(p))
                    return p->getCode();
            }
            return "";
        }

        case COMDifficulty::MEDIUM: {
            Property* chosen = chooseAnyFestivalProperty(gs);
            if (chosen) {
                announceAction("Festival: memilih " + chosen->getName() + ".");
                return chosen->getCode();
            }
            return "";
        }

        case COMDifficulty::HARD: {
            Property* chosen = chooseBestFestivalProperty(gs);
            if (chosen) {
                announceAction("Festival: memilih " + chosen->getName() + " (sewa tertinggi).");
                return chosen->getCode();
            }
            return "";
        }
    }
    return "";
}

bool ComputerPlayer::decidePayJailFine(GameState& gs) {
    int fine = 50; // default; idealnya dibaca dari konfigurasi via gs
    // Coba ambil fine dari JailTile
    Board* board = gs.getBoard();
    if (board) {
        for (int i = 0; i < board->getSize(); i++) {
            JailTile* jt = dynamic_cast<JailTile*>(board->getTile(i));
            if (jt) { fine = jt->getJailFine(); break; }
        }
    }

    switch (difficulty) {
        case COMDifficulty::EASY:
            // 50% chance bayar, 50% chance coba double
            if (randomChance(0.5) && canAfford(fine)) {
                announceAction("Membayar denda penjara M" + std::to_string(fine) + " (acak).");
                return true;
            }
            announceAction("Mencoba lempar double untuk keluar penjara (acak).");
            return false;

        case COMDifficulty::MEDIUM:
            // Bayar jika sudah 2+ giliran di penjara atau sangat mampu
            if (getJailTurns() >= 2 && canAfford(fine)) {
                announceAction("Membayar denda penjara (sudah " +
                    std::to_string(getJailTurns()) + " giliran dalam penjara).");
                return true;
            }
            announceAction("Mencoba lempar double untuk keluar penjara.");
            return false;

        case COMDifficulty::HARD:
            // Awal permainan: sengaja di penjara (aman dari sewa mahal)
            // Akhir permainan: bayar untuk tetap aktif
            if (getJailTurns() >= 2 || getBalance() > 800) {
                if (canAfford(fine)) {
                    announceAction("Membayar denda penjara M" + std::to_string(fine) +
                        " (keputusan strategis: tetap aktif).");
                    return true;
                }
            }
            announceAction("Bertahan di penjara sementara — aman dari sewa.");
            return false;
    }
    return false;
}

bool ComputerPlayer::decideIncomeTaxChoice(GameState& gs) {
    // true = bayar flat, false = bayar persentase kekayaan
    switch (difficulty) {
        case COMDifficulty::EASY:
            // Selalu pilih flat (tidak mau menghitung)
            announceAction("Memilih PPH flat (tanpa perhitungan).");
            return true;

        case COMDifficulty::MEDIUM: {
            // Bandingkan flat vs persentase, pilih yang lebih kecil
            // Asumsi flat = 150, persentase = 10% (default konfigurasi)
            int flat       = 150;
            double pct     = 0.10;
            int wealth     = getWealth();
            int pctAmount  = static_cast<int>(wealth * pct);
            bool useFlat   = (flat <= pctAmount);
            announceAction(std::string("Memilih PPH ") +
                (useFlat ? "flat M150" : "10% kekayaan (M" + std::to_string(pctAmount) + ")") +
                " — lebih hemat.");
            return useFlat;
        }

        case COMDifficulty::HARD: {
            // Kalkulasi tepat: pilih yang lebih kecil
            int flat      = 150;
            double pct    = 0.10;
            int wealth    = getWealth();
            int pctAmount = static_cast<int>(wealth * pct);
            bool useFlat  = (flat <= pctAmount);
            announceAction(std::string("Kalkulasi PPH: flat M") + std::to_string(flat) +
                " vs persentase M" + std::to_string(pctAmount) +
                " → memilih " + (useFlat ? "flat" : "persentase") + ".");
            return useFlat;
        }
    }
    return true;
}

int ComputerPlayer::decideUseSkillCard(GameState& gs) {
    const std::vector<SkillCard*>& hand = getHand();
    if (hand.empty()) return -1;

    switch (difficulty) {
        case COMDifficulty::EASY:
            // 20% chance pakai kartu pertama secara acak
            if (randomChance(0.20)) {
                announceAction("Menggunakan kartu kemampuan (acak): " + hand[0]->getDescription());
                return 0;
            }
            return -1;

        case COMDifficulty::MEDIUM: {
            // Prioritas: ShieldCard jika saldo rendah, MoveCard jika dekat GO
            for (int i = 0; i < static_cast<int>(hand.size()); i++) {
                ShieldCard* sc = dynamic_cast<ShieldCard*>(hand[i]);
                if (sc && getBalance() < 300) {
                    announceAction("Menggunakan ShieldCard — saldo rendah, butuh perlindungan.");
                    return i;
                }
            }
            return -1;
        }

        case COMDifficulty::HARD: {
            // Prioritas:
            // 1. TeleportCard jika ada properti berharga yang belum dimiliki
            // 2. ShieldCard jika di area properti mahal lawan
            // 3. MoveCard untuk menghindari petak berbahaya
            // 4. DiscountCard sebelum membeli properti mahal
            Board* board = gs.getBoard();
            int curPos = getPosition();

            for (int i = 0; i < static_cast<int>(hand.size()); i++) {
                // ShieldCard: pakai jika saldo < 400
                ShieldCard* sc = dynamic_cast<ShieldCard*>(hand[i]);
                if (sc && getBalance() < 400) {
                    announceAction("Menggunakan ShieldCard — saldo rendah, perlindungan strategis.");
                    return i;
                }
                // MoveCard: pakai jika posisi dekat petak pajak atau penjara
                MoveCard* mc = dynamic_cast<MoveCard*>(hand[i]);
                if (mc && board) {
                    // Periksa 1-6 petak ke depan: hindari tax dan go-to-jail
                    for (int step = 1; step <= 6; step++) {
                        int nextPos = (curPos + step) % board->getSize();
                        Tile* t = board->getTile(nextPos);
                        if (dynamic_cast<TaxTile*>(t)) {
                            announceAction("Menggunakan MoveCard — menghindari petak pajak di depan.");
                            return i;
                        }
                    }
                }
            }
            return -1;
        }
    }
    return -1;
}

int ComputerPlayer::decideDropCard(const std::vector<SkillCard*>& hand,
                                    SkillCard* newCard) const {
    switch (difficulty) {
        case COMDifficulty::EASY:
            // Buang kartu pertama
            announceAction("Membuang kartu ke-0 (acak): " + (hand.empty() ? "?" : hand[0]->getDescription()));
            return 0;

        case COMDifficulty::MEDIUM: {
            // Buang kartu dengan nilai paling rendah (MoveCard 1 langkah, dll.)
            // Prioritas buang: kartu yang sudah expired atau nilai kecil
            // Implementasi sederhana: buang indeks 0
            return 0;
        }

        case COMDifficulty::HARD: {
            // Buang kartu dengan tipe paling tidak berguna saat ini
            // Urutan kepentingan (dari paling berharga): Shield > Teleport > Lasso > Demolition > Discount > Move
            // Buang yang paling akhir di urutan ini
            static const std::vector<std::string> priority = {
                "ShieldCard", "TeleportCard", "LassoCard",
                "DemolitionCard", "DiscountCard", "MoveCard"
            };
            int dropIdx = 0;
            int lowestPriority = -1;
            for (int i = 0; i < static_cast<int>(hand.size()); i++) {
                std::string type = hand[i]->getType();
                int prio = static_cast<int>(priority.size()); // default: sangat rendah
                for (int j = 0; j < static_cast<int>(priority.size()); j++) {
                    if (priority[j] == type) { prio = j; break; }
                }
                if (prio > lowestPriority) {
                    lowestPriority = prio;
                    dropIdx        = i;
                }
            }
            announceAction("Membuang " + hand[dropIdx]->getDescription() + " (prioritas terendah).");
            return dropIdx;
        }
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────
//  executeTurn — menjalankan seluruh giliran COM secara otomatis
// ─────────────────────────────────────────────────────────────────

void ComputerPlayer::executeTurn(GameMaster& gm) {
    GameState& gs = gm.getState();
    std::cout << "\n[COM - " << getUsername()
              << " (" << difficultyToString(difficulty) << ")] Giliran dimulai."
              << " | Saldo: M" << getBalance()
              << " | Properti: " << getPropertyCount() << std::endl;

// ── 1. Gunakan Skill Card (sebelum lempar dadu) ──────────────────
if (!gs.getHasUsedCard() && !getHand().empty()) {
    int cardIdx = decideUseSkillCard(gs);
    if (cardIdx >= 0 && cardIdx < getHandSize()) {
        SkillCard* chosen = getHand()[cardIdx];
        if (chosen) {
            std::string type = chosen->getType();
            
            // LassoCard — pilih target dulu
            if (type == "LassoCard") {
                auto* lasso = dynamic_cast<LassoCard*>(chosen);
                if (lasso) {
                    // Pilih target: player dengan properti terbanyak
                    Player* target = nullptr;
                    int maxProp = 0;
                    for (Player* p : gs.getActivePlayers()) {
                        if (p == this) continue;
                        if (p->getPropertyCount() > maxProp) {
                            maxProp = p->getPropertyCount();
                            target = p;
                        }
                    }
                    if (target) {
                        lasso->setTargetPlayerUsername(target->getUsername());
                        gm.useSkillCard(this, chosen, gs);
                        gs.setHasUsedCard(true);
                    }
                }
            }
            // TeleportCard — pilih tile properti bank terdekat
            else if (type == "TeleportCard") {
                auto* teleport = dynamic_cast<TeleportCard*>(chosen);
                if (teleport) {
                    Board* board = gs.getBoard();
                    int bestPos = -1;
                    int bestScore = -1;
                    if (board) {
                        for (int i = 0; i < board->getSize(); i++) {
                            auto* pt = dynamic_cast<PropertyTile*>(board->getTile(i));
                            if (!pt || !pt->getProperty()) continue;
                            if (pt->getProperty()->getStatus() == PropertyStatus::BANK) {
                                int price = pt->getProperty()->getPurchasePrice();
                                if (price > bestScore && canAfford(price)) {
                                    bestScore = price;
                                    bestPos = i;
                                }
                            }
                        }
                    }
                    if (bestPos >= 0) {
                        teleport->setTargetPosition(bestPos, gs);
                        gm.useSkillCard(this, chosen, gs);
                        gs.setHasUsedCard(true);
                    }
                }
            }
            // Kartu lain — langsung pakai
            else {
                gm.useSkillCard(this, chosen, gs);
                gs.setHasUsedCard(true);
            }
        }
    }
}

    // ── 2. Tangani penjara ───────────────────────────────────────────
    if (isInJail()) {
        bool payFine = decidePayJailFine(gs);
        if (payFine) {
            // Bayar denda dan keluar
            Board* board = gs.getBoard();
            if (board) {
                for (int i = 0; i < board->getSize(); i++) {
                    JailTile* jt = dynamic_cast<JailTile*>(board->getTile(i));
                    if (jt) {
                        jt->payFine(*this, gs);
                        // Setelah bayar denda, lempar dadu dan bergerak
                        gs.getDice()->rollRandom();
                        int total = gs.getDice()->getTotal();
                        std::cout << "[COM] Lempar dadu: "
                                  << gs.getDice()->getDaduVal1() << " + "
                                  << gs.getDice()->getDaduVal2()
                                  << " = " << total << std::endl;
                        gm.movePlayer(this, total);
                        break;
                    }
                }
            }
        } else {
            // Coba lempar double
            Dice* dice = gs.getDice();
            if (dice) {
                dice->rollRandom();
                std::cout << "[COM] Mencoba double di penjara: "
                          << dice->getDaduVal1() << " + "
                          << dice->getDaduVal2() << " = " << dice->getTotal();
                if (dice->isDouble()) {
                    std::cout << " (DOUBLE! Keluar penjara)" << std::endl;
                    gm.releaseFromJail(this);
                    gm.movePlayer(this, dice->getTotal());
                } else {
                    std::cout << " (Gagal keluar)" << std::endl;
                    incrementJailTurns();
                    // Pada giliran ke-4 (jailTurns==3), wajib bayar
                    if (getJailTurns() >= 3) {
                        announceAction("Giliran ke-4 di penjara — wajib bayar denda.");
                        Board* board = gs.getBoard();
                        if (board) {
                            for (int i = 0; i < board->getSize(); i++) {
                                JailTile* jt = dynamic_cast<JailTile*>(board->getTile(i));
                                if (jt) { jt->payFine(*this, gs); break; }
                            }
                        }
                        gs.getDice()->rollRandom();
                        gm.movePlayer(this, gs.getDice()->getTotal());
                    }
                }
            }
        }
        gs.setPhase(GamePhase::PLAYER_TURN); // siap endTurn
        return;
    }

    // ── 3. Lempar dadu normal ────────────────────────────────────────
    bool keepRolling = true;
    int  rollCount   = 0;

    while (keepRolling) {
        Dice* dice = gs.getDice();
        if (!dice) break;

        dice->rollRandom();
        int d1    = dice->getDaduVal1();
        int d2    = dice->getDaduVal2();
        int total = dice->getTotal();

        std::cout << "[COM] Lempar dadu ke-" << (rollCount + 1) << ": "
                  << d1 << " + " << d2 << " = " << total << std::endl;

        // Triple double → penjara
        if (dice->isDouble() && dice->getConsecutiveDoubles() >= 3) {
            announceAction("Triple double! Masuk penjara.");
            gm.sendPlayerToJail(this);
            return;
        }

        // Gerakkan pemain
        gm.movePlayer(this, total);
        rollCount++;

        // Ekstra turn jika double
        if (dice->isDouble()) {
            announceAction("Double! Mendapat giliran tambahan.");
            keepRolling = true;
        } else {
            keepRolling = false;
        }
    }

    // ── 4. Tangani Interaksi (Beli, Pajak, Festival, dsb.) ─────────
    GamePhase currentPhase = gs.getPhase();
    
    // Jika phase masih interaktif, COM harus mengambil keputusan
    if (currentPhase == GamePhase::AWAITING_BUY) {
        Tile* landed = gs.getBoard()->getTile(getPosition());
        PropertyTile* pt = dynamic_cast<PropertyTile*>(landed);
        if (pt && pt->getProperty()) {
            bool wantToBuy = decideAndBuy(pt->getProperty(), gs);
            BeliCommand cmd(this, pt->getProperty(), wantToBuy);
            cmd.execute(gm);
        }
    } else if (currentPhase == GamePhase::AWAITING_FESTIVAL) {
        Property* best = chooseBestFestivalProperty(gs);
        if (best) {
            StreetProperty* sp = dynamic_cast<StreetProperty*>(best);
            if (sp) {
                FestivalCommand cmd(this, gs.getLogger(), gs.getCurrTurn());
                cmd.executeWithProperty(sp);
            }
        }
        gs.setPhase(GamePhase::PLAYER_TURN);
    } else if (currentPhase == GamePhase::AWAITING_TAX) {
        // COM pilih yang termurah antara flat vs percentage
        int taxFlat = gs.getPendingPphFlat();
        int taxPct = (int)(gm.calculateWealth(this) * (gs.getPendingPphPct() / 100.0));
        
        // Pura-pura panggil logic bayar pajak (Command)
        // Disini kita sederhanakan:
        if (taxFlat < taxPct) {
            *this -= taxFlat;
            gm.log(getUsername(), "TAX", "Bayar PPH Flat: M" + std::to_string(taxFlat));
        } else {
            *this -= taxPct;
            gm.log(getUsername(), "TAX", "Bayar PPH %: M" + std::to_string(taxPct));
        }
        gs.setPhase(GamePhase::PLAYER_TURN);
    } else if (currentPhase == GamePhase::SHOW_CARD) {
        // COM hanya skip card dialog
        gs.setPhase(GamePhase::PLAYER_TURN);
    }

    // ── 5. Aksi pasca-gerak: bangun & tebus ─────────────────────────
    //  (hanya MEDIUM dan HARD)
    if (difficulty != COMDifficulty::EASY && gs.getPhase() == GamePhase::PLAYER_TURN) {
        // Coba bangun
        auto [colorGroup, propCode] = decideBuild(gs);
        if (!propCode.empty()) {
            // Cari properti yang dimaksud dan bangun
            for (Property* p : getProperties()) {
                if (p && p->getCode() == propCode) {
                    StreetProperty* sp = dynamic_cast<StreetProperty*>(p);
                    if (sp && canAfford(sp->getHouseUpgCost())) {
                        sp->buildHouse();
                        *this -= sp->getHouseUpgCost();
                        gm.log(getUsername(), "BANGUN",
                            "Membangun rumah di " + p->getName() +
                            " seharga M" + std::to_string(sp->getHouseUpgCost()));
                    }
                    break;
                }
            }
        }

        // Coba tebus properti yang sedang digadai
        for (Property* p : getProperties()) {
            if (!p || p->getStatus() != PropertyStatus::MORTGAGED) continue;
            if (decideRedeem(p, gs)) {
                int redeemCost = p->getPurchasePrice();
                if (canAfford(redeemCost)) {
                    *this -= redeemCost;
                    p->setStatus(PropertyStatus::OWNED);
                    gm.log(getUsername(), "TEBUS",
                        "Menebus " + p->getName() +
                        " seharga M" + std::to_string(redeemCost));
                }
            }
        }
    }

    gs.setPhase(GamePhase::PLAYER_TURN); // siap endTurn
}

ComputerPlayer::~ComputerPlayer() {}