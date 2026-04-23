#include "GameMaster.hpp"
#include "../Player/Player.hpp"
#include "../Board/Board.hpp"
#include "../Bank/Bank.hpp"
#include "../Dice/Dice.hpp"
#include "../Card/SkillCard.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../Property/Property.hpp"
#include "../utils/TransactionLogger.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

GameMaster::GameMaster(GameState initialState)
    : state(std::move(initialState))
{
    state.setGameMaster(this);
}

// ─────────────────────────────────────────────
//  Akses state
// ─────────────────────────────────────────────

GameState &GameMaster::getState() { return state; }
const GameState &GameMaster::getState() const { return state; }

// ─────────────────────────────────────────────
//  Main loop (CLI)
// ─────────────────────────────────────────────

void GameMaster::start()
{
    state.setPhase(GamePhase::PLAYER_TURN);
    std::cout << "=== Nimonspoli Dimulai ===" << std::endl;

    while (!state.isGameOver())
    {
        beginTurn();

        // Loop input perintah dalam satu giliran
        while (state.getPhase() == GamePhase::PLAYER_TURN ||
               state.getPhase() == GamePhase::DICE_ROLLED)
        {

            Player *cur = state.getCurrPlayer();
            if (!cur || cur->getStatus() == PlayerStatus::BANKRUPT)
                break;

            std::cout << "\n[Turn " << state.getCurrTurn() << "] "
                      << cur->getUsername() << " > ";

            std::string input;
            if (!std::getline(std::cin, input))
                break;
            if (!input.empty())
                handleCommand(input);
        }

        endTurn();
        checkWinCondition();
    }

    handleWinner();
}

void GameMaster::handleCommand(const std::string &rawInput)
{
    // Parsing dan dispatch ke Command object dilakukan di lapisan CLI / GUI
    // GameMaster hanya menyediakan method; CommandDispatcher yang memanggil
    (void)rawInput; // placeholder — diisi oleh tim CLI/GUI
}

void GameMaster::beginTurn()
{
    state.setPhase(GamePhase::PLAYER_TURN);
    state.setHasRolled(false);
    state.setHasUsedCard(false);
    state.setHasExtraTurn(false);

    distributeSkillCards();

    Player *cur = state.getCurrPlayer();
    if (cur)
    {
        log(cur->getUsername(), "TURN_START",
            "Giliran Turn " + std::to_string(state.getCurrTurn()));
    }
}

void GameMaster::endTurn()
{
    tickFestivalDurations();

    if (!state.getHasExtraTurn())
    {
        // Satu siklus penuh (semua pemain sudah jalan) → naikkan turn
        int prevIdx = state.getCurrPlayerIdx();
        state.nextPlayer();
        if (state.getCurrPlayerIdx() <= prevIdx)
        {
            // Sudah berputar penuh
            state.advanceTurn();
        }
    }
    // Jika hasExtraTurn, pemain yang sama jalan lagi — tidak nextPlayer()
}

// ─────────────────────────────────────────────
//  Pergerakan pemain
// ─────────────────────────────────────────────
void GameMaster::movePlayer(Player *player, int steps)
{
    if (!player || !state.getBoard())
        return;

    Board *board = state.getBoard();

    int curIdx = player->getPosition();
    int goIdx = 1; // GO selalu di id=1

    // Hitung target — wrap dalam range 1..40
    int targetIdx = curIdx + steps;
    bool passedGo = false;

    if (targetIdx > 40)
    {
        targetIdx = ((targetIdx - 1) % 40) + 1; // wrap 1-40
        passedGo = true;
    }

    // Bayar gaji GO
    if (passedGo)
    {
        Tile *goTile = board->getTile(goIdx);
        GoTile *go = dynamic_cast<GoTile *>(goTile);
        if (go)
        {
            state.getBank()->payPlayer(player, go->getSalary());
            log(player->getUsername(), "GO_SALARY",
                "Melewati GO, menerima M" + std::to_string(go->getSalary()));
        }
    }

    player->setPosition(targetIdx);

    Tile *landedTile = board->getTile(targetIdx);
    if (landedTile)
    {
        log(player->getUsername(), "MOVE",
            "Mendarat di " + landedTile->getCode() +
                " (" + landedTile->getTileName() + ")");
        landedTile->onLanded(*player, state);
    }
}

void GameMaster::teleportPlayer(Player *player, int targetIndex, bool passThroughGo)
{
    if (!player || !state.getBoard())
        return;

    Board *board = state.getBoard();
    int boardSize = board->getSize();
    if (targetIndex < 0 || targetIndex >= boardSize)
        return;

    // Cari posisi saat ini
    int curIdx = player->getPosition();

    // Bayar gaji GO jika melewati (hanya jika bukan pergi ke penjara)
    if (passThroughGo && targetIndex < curIdx)
    {
        Tile *goTile = board->getTile(0);
        GoTile *go = dynamic_cast<GoTile *>(goTile);
        if (go)
        {
            state.getBank()->payPlayer(player, go->getSalary());
            log(player->getUsername(), "GO_SALARY",
                "Melewati GO, menerima M" + std::to_string(go->getSalary()));
        }
    }

    player->setPosition(targetIndex);

    Tile *landedTile = board->getTile(targetIndex);
    if (landedTile)
    {
        log(player->getUsername(), "TELEPORT",
            "Dipindahkan ke " + landedTile->getCode());
        landedTile->onLanded(*player, state);
    }
}

// ─────────────────────────────────────────────
//  Penjara
// ─────────────────────────────────────────────

JailTile *GameMaster::findJailTile() const
{
    Board *board = state.getBoard();
    if (!board)
        return nullptr;
    for (int i = 0; i < board->getSize(); i++)
    {
        JailTile *jt = dynamic_cast<JailTile *>(board->getTile(i));
        if (jt)
            return jt;
    }
    return nullptr;
}

int GameMaster::findJailIndex() const
{
    Board *board = state.getBoard();
    if (!board)
        return -1;
    for (int i = 0; i < board->getSize(); i++)
    {
        if (dynamic_cast<JailTile *>(board->getTile(i)))
            return i;
    }
    return -1;
}

void GameMaster::sendPlayerToJail(Player *player)
{
    if (!player)
        return;

    JailTile *jail = findJailTile();
    if (jail)
    {
        jail->sendToJail(*player);
        log(player->getUsername(), "JAIL",
            player->getUsername() + " dimasukkan ke penjara!");
    }

    // flag JAILED sudah di-set oleh JailTile::sendToJail()
    endCurrentTurn();
}

bool GameMaster::releaseFromJail(Player *player)
{
    if (!player)
        return false;
    JailTile *jail = findJailTile();
    if (!jail || !jail->isInmate(*player))
        return false;
    jail->release(*player);
    log(player->getUsername(), "RELEASE", player->getUsername() + " keluar dari penjara.");
    return true;
}

// ─────────────────────────────────────────────
//  Kontrol giliran
// ─────────────────────────────────────────────

void GameMaster::setExtraTurn(bool val)
{
    state.setHasExtraTurn(val);
}

void GameMaster::endCurrentTurn()
{
    state.setHasExtraTurn(false);
    state.setPhase(GamePhase::PLAYER_TURN);
    // Paksa pindah ke pemain berikutnya
    state.nextPlayer();
}

bool GameMaster::hasExtraTurn() const
{
    return state.getHasExtraTurn();
}

// ─────────────────────────────────────────────
//  Properti
// ─────────────────────────────────────────────

void GameMaster::handlePropertyLanding(Player *player, Property *prop)
{
    if (!player || !prop)
        return;

    if (prop->getStatus() == PropertyStatus::BANK)
    {
        // Tawarkan pembelian (Street) atau beri gratis (Railroad/Utility)
        // Detail logika ada di BeliCommand — di sini hanya trigger
        // BeliCommand akan dipanggil dari onLanded() masing-masing tile
    }
    else if (prop->getStatus() == PropertyStatus::OWNED)
    {
        if (prop->getOwnerId() != player->getUsername())
        {
            // Bayar sewa — BayarSewaCommand dipanggil dari onLanded()
        }
    }
    // MORTGAGED → tidak ada aksi
}

void GameMaster::startAuction(Property *prop, Player *triggerPlayer)
{
    if (!prop)
        return;

    AuctionManager *am = state.getAuctionManager();
    std::vector<Player *> all = state.getActivePlayers();

    // Susun urutan lelang: mulai dari pemain setelah trigger
    std::vector<Player *> participants;
    if (triggerPlayer)
    {
        auto it = std::find(all.begin(), all.end(), triggerPlayer);
        if (it != all.end())
        {
            ++it;
            while (it != all.end())
            {
                participants.push_back(*it++);
            }
            it = all.begin();
            while (*it != triggerPlayer)
            {
                participants.push_back(*it++);
            }
        }
    }
    else
    {
        participants = all;
    }

    am->setupAuction(prop, participants);
    state.setPhase(GamePhase::AUCTION);

    log("SYSTEM", "AUCTION_START",
        "Lelang dimulai untuk " + prop->getName());

    // Loop lelang — setiap giliran pemain: BID atau PASS
    // (Detail interaksi I/O dilakukan oleh LelangCommand)
    // GameMaster hanya menyediakan am->placeBid() dan am->closeAuction()
}

// ─────────────────────────────────────────────
//  Kebangkrutan
// ─────────────────────────────────────────────

void GameMaster::handleDebtPayment(Player *debtor, int debt, Player *creditor)
{
    if (!debtor)
        return;

    // Hitung maksimum yang bisa didapat dari likuidasi
    // (perhitungan detail ada di BankruptcyManager / BangkrutCommand)
    int cash = debtor->getBalance();
    if (cash >= debt)
    {
        // Cukup bayar langsung
        if (creditor)
        {
            *debtor -= debt;
            *creditor += debt;
        }
        else
        {
            *debtor -= debt;
        }
        return;
    }

    // Tidak cukup cash → cek potensi likuidasi
    int potential = calculateWealth(debtor);
    if (potential >= debt)
    {
        // Wajib likuidasi — BangkrutCommand yang handle panel likuidasi
        state.setPhase(GamePhase::BANKRUPTCY);
        log(debtor->getUsername(), "BANKRUPTCY_START",
            "Harus likuidasi untuk bayar M" + std::to_string(debt));
    }
    else
    {
        // Tidak bisa bayar → bangkrut
        if (creditor)
        {
            handleBankruptcy(debtor, creditor);
        }
        else
        {
            handleBankruptcy(debtor, state.getBank());
        }
    }
}

void GameMaster::handleBankruptcy(Player *from, Player *to)
{
    if (!from || !to)
        return;

    log(from->getUsername(), "BANKRUPT",
        from->getUsername() + " bangkrut ke " + to->getUsername());

    // Pindahkan uang sisa
    int remaining = from->getBalance();
    if (remaining > 0)
    {
        *from -= remaining;
        *to += remaining;
    }

    // Pindahkan semua properti (termasuk yang MORTGAGED, tetap dalam kondisi gadai)
    for (int i = 0; i < from->getPropertyCount(); i++)
    {
        Property *p = from->getProperties()[i];
        if (p)
        {
            p->setOwner(to->getUsername());
            to->addProperty(p);
        }
    }

    // Tandai bankrut & keluarkan dari urutan giliran
    state.removePlayer(from);

    // Cek apakah permainan selesai
    if (state.countActivePlayers() <= 1)
    {
        state.setPhase(GamePhase::GAME_OVER);
    }
}

void GameMaster::handleBankruptcy(Player *from, Bank *bank)
{
    if (!from || !bank)
        return;

    log(from->getUsername(), "BANKRUPT",
        from->getUsername() + " bangkrut ke Bank");

    // Serahkan uang sisa ke Bank (hilang dari peredaran)
    int remaining = from->getBalance();
    if (remaining > 0)
    {
        from -= remaining;
    }

    // Semua properti kembali ke BANK dan dilelang
    for (int i = 0; i < from->getPropertyCount(); i++)
    {
        Property *p = from->getProperties()[i];
        if (p)
        {
            p->clearOwner();
            p->setStatus(PropertyStatus::BANK);
            // Hancurkan bangunan jika ada (StreetProperty)
            // → dilakukan di StreetProperty::resetBuildings() jika ada
            startAuction(p, nullptr);
        }
    }

    state.removePlayer(from);

    if (state.countActivePlayers() <= 1)
    {
        state.setPhase(GamePhase::GAME_OVER);
    }
}

// ─────────────────────────────────────────────
//  Kemenangan
// ─────────────────────────────────────────────

void GameMaster::handleWinner()
{
    std::vector<Player *> active = state.getActivePlayers();

    if (active.size() == 1)
    {
        // Menang karena bankruptcy
        std::cout << "\n=== Permainan Selesai (Bankruptcy) ===" << std::endl;
        std::cout << "Pemenang: " << active[0]->getUsername() << std::endl;
        return;
    }

    // Menang karena MAX_TURN — tiebreak: uang → properti → kartu
    std::sort(active.begin(), active.end(), [this](Player *a, Player *b)
              {
                  if (a->getBalance() != b->getBalance())
                      return a->getBalance() > b->getBalance();
                  if (a->getPropertyCount() != b->getPropertyCount())
                      return a->getPropertyCount() > b->getPropertyCount();
                  // Jumlah kartu — akses via getCardAt dengan loop
                  // (Tidak ada getCardNum() di Player; gunakan getPropertyNum sebagai fallback)
                  return false; // seri → semua menang
              });

    std::cout << "\n=== Permainan Selesai (Max Turn) ===" << std::endl;
    std::cout << "Rekap Pemain:" << std::endl;
    for (Player *p : active)
    {
        std::cout << "  " << p->getUsername()
                  << " | Uang: M" << p->getBalance()
                  << " | Properti: " << p->getPropertyCount()
                  << std::endl;
    }
    std::cout << "Pemenang: " << active[0]->getUsername() << std::endl;
}

// ─────────────────────────────────────────────
//  Helper: query state
// ─────────────────────────────────────────────

bool GameMaster::hasMonopoly(Player *player, const std::string &colorGroup) const
{
    if (!player || !state.getBoard())
        return false;

    Board *board = state.getBoard();
    int owned = 0, total = 0;

    for (int i = 0; i < board->getSize(); i++)
    {
        PropertyTile *pt = dynamic_cast<PropertyTile *>(board->getTile(i));
        if (!pt)
            continue;
        Property *prop = pt->getProperty();
        if (!prop || prop->getColorGroup() != colorGroup)
            continue;
        total++;
        if (prop->getOwnerId() == player->getUsername())
            owned++;
    }

    return total > 0 && owned == total;
}

int GameMaster::countPlayerRailroads(Player *player) const
{
    if (!player || !state.getBoard())
        return 0;
    Board *board = state.getBoard();
    int count = 0;
    for (int i = 0; i < board->getSize(); i++)
    {
        RailRoadTile *rt = dynamic_cast<RailRoadTile *>(board->getTile(i));
        if (!rt)
            continue;
        Property *prop = rt->getProperty();
        if (prop && prop->getOwnerId() == player->getUsername())
            count++;
    }
    return count;
}

int GameMaster::countPlayerUtilities(Player *player) const
{
    if (!player || !state.getBoard())
        return 0;
    Board *board = state.getBoard();
    int count = 0;
    for (int i = 0; i < board->getSize(); i++)
    {
        UtilityTile *ut = dynamic_cast<UtilityTile *>(board->getTile(i));
        if (!ut)
            continue;
        Property *prop = ut->getProperty();
        if (prop && prop->getOwnerId() == player->getUsername())
            count++;
    }
    return count;
}

int GameMaster::findNearestRailroad(int currentPosition) const
{
    Board *board = state.getBoard();
    if (!board)
        return -1;

    int size = board->getSize();
    for (int step = 1; step < size; step++)
    {
        int idx = (currentPosition + step) % size;
        if (dynamic_cast<RailRoadTile *>(board->getTile(idx)))
            return idx;
    }
    return -1;
}

int GameMaster::calculateWealth(Player *player) const
{
    if (!player)
        return 0;
    int wealth = player->getBalance();
    for (int i = 0; i < player->getPropertyCount(); i++)
    {
        Property *p = player->getProperties()[i];
        if (p)
            wealth += static_cast<int>(p->getPurchasePrice());
        // Nilai bangunan ditambahkan oleh StreetProperty::calculateSellPrice()
        // jika ada override — di sini gunakan purchasePrice sebagai baseline
    }
    return wealth;
}

void GameMaster::log(const std::string &username,
                     const std::string &action,
                     const std::string &detail)
{
    TransactionLogger *logger = state.getLogger();
    if (logger)
    {
        logger->addLog(state.getCurrTurn(), username, action, detail);
    }
}

// ─────────────────────────────────────────────
//  Helper internal
// ─────────────────────────────────────────────

void GameMaster::distributeSkillCards()
{
    // Setiap pemain aktif dapat 1 kartu dari skillDeck
    CardDeck<Card> *deck = state.getSkillDeck();
    if (!deck)
        return;

    for (Player *p : state.getActivePlayers())
    {
        if (!p)
            continue;
        // Jika tangan sudah 3 kartu, flag akan di-handle oleh DropKartuCommand
        // (dipicu otomatis dari sini atau dari Command dispatcher)
        // Di sini cukup addCard; overflow check ada di Player atau Command
        Card *drawn = deck->draw();
        if (drawn)
            p->addSkillCard(dynamic_cast<SkillCard *>(drawn));
    }
}

void GameMaster::tickFestivalDurations()
{
    // Kurangi durasi festival untuk semua properti milik pemain aktif
    // StreetProperty harus punya method tickFestival() / decreaseFestivalDuration()
    Player *cur = state.getCurrPlayer();
    if (!cur)
        return;

    for (int i = 0; i < cur->getPropertyCount(); i++)
    {
        Property *p = cur->getProperties()[i];
        if (!p)
            continue;
        // Cast ke StreetProperty jika ada method festivalnya
        // StreetProperty* sp = dynamic_cast<StreetProperty*>(p);
        // if (sp) sp->tickFestival();
        // → Uncomment setelah StreetProperty diimplementasi
    }
}

void GameMaster::checkWinCondition()
{
    // Kondisi 1: hanya 1 pemain aktif tersisa
    if (state.countActivePlayers() <= 1)
    {
        state.setPhase(GamePhase::GAME_OVER);
        return;
    }

    // Kondisi 2: MAX_TURN tercapai
    if (state.isMaxTurnReached())
    {
        state.setPhase(GamePhase::GAME_OVER);
        return;
    }

    // Kondisi 3: maxTurn < 1 → mode BANKRUPTCY, game terus tanpa batas
}

void GameMaster::useSkillCard(Player *player, SkillCard *card, GameState &gs)
{
    if (!player || !card)
        return;
    if (gs.getHasUsedCard())
        return;
    if (card->isUsed())
        return;

    card->execute(*player, gs);
    gs.getLogger()->addLog(gs.getCurrTurn(), player->getUsername(), "SKILL_CARD", card->getDescription());

    card->markUsed();
    const vector<SkillCard *> &hand = player->getHand();
    for (int i = 0; i < (int)hand.size(); i++)
    {
        if (hand[i] == card)
        {
            player->discardSkillCard(i);
            break;
        }
    }
    gs.setHasUsedCard(true);
}