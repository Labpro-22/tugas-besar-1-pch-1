#include "GameMaster.hpp"
#include "../Player/Player.hpp"
#include "../Board/Board.hpp"
#include "../Bank/Bank.hpp"
#include "../Dice/Dice.hpp"
#include "../Card/SkillCard.hpp"
#include "../Card/DiscountCard.hpp"
#include "../Card/CardDeck.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../Property/Property.hpp"
#include "../utils/TransactionLogger.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <limits>

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
    bool extraTurn = state.getHasExtraTurn();
    state.setPhase(GamePhase::PLAYER_TURN);
    state.setHasRolled(false);
    state.setHasUsedCard(false);

    Player *cur = state.getCurrPlayer();
    if (!cur || cur->getStatus() == PlayerStatus::BANKRUPT)
        return;
    cur->onTurnStart();
    if (!extraTurn)
        distributeSkillCards();
    if (cur->isInJail())
    {
        state.setPhase(GamePhase::AWAITING_JAIL);
    }
    log(cur->getUsername(), "TURN_START",
        "Giliran Turn " + std::to_string(state.getCurrTurn()));
}

void GameMaster::endTurn()
{
    tickFestivalDurations();
    tickDiscountDurations();
    if (!state.getHasExtraTurn())
    {
        int prevIdx = state.getCurrPlayerIdx();
        state.nextPlayer();
        if (state.getCurrPlayerIdx() <= prevIdx)
        {
            state.advanceTurn();
        }
    }

    checkWinCondition();
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
    int boardSize = state.getBoard()->getSize();

    int targetIdx = (curIdx + steps) % boardSize;

    // detect pass GO (GO = index 0)
    bool passedGo = (curIdx + steps) >= boardSize;

    if (passedGo)
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
    log(player->getUsername(), "DADU",
        "Berangkat dari tile ke-" + std::to_string(curIdx) + " dengan langkah sebesar " + std::to_string(steps) + " menuju tile ke-" + std::to_string(targetIdx));
    player->setPosition(targetIdx);

    Tile *landedTile = board->getTile(targetIdx);
    if (landedTile)
    {
        log(player->getUsername(), "DADU",
            "Mendarat di " + landedTile->getCode() +
                " (" + landedTile->getTileName() + ")");
        landedTile->onLanded(*player, state);
        if (state.getPhase() != GamePhase::PLAYER_TURN &&
            state.getPhase() != GamePhase::DICE_ROLLED)
        {
            return; // STOP di sini → tunggu dialog selesai
        }
    }
    else
    {
        log(player->getUsername(), "DADU",
            "Anda tidak mendarat di mana pun");
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
    if(player->isShielded())return;
    int jailIdx = findJailIndex();
    if (jailIdx >= 0)
        player->setPosition(jailIdx);

    Dice *dice = state.getDice();
    if (dice)
        dice->resetConsecutiveDoubles(); // FIX

    state.setHasRolled(true); // FIX: stop movement

    JailTile *jail = findJailTile();
    if (jail)
    {
        jail->sendToJail(*player);
        log(player->getUsername(), "JAIL",
            player->getUsername() + " dimasukkan ke penjara!");
    }

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
    state.setHasRolled(true);
    state.setPhase(GamePhase::PLAYER_TURN);

    // Cek win condition di sini agar GUI flow juga mendeteksi game over
    checkWinCondition();
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
    if (!am)
        return;

    std::vector<Player *> all = state.getActivePlayers();
    if (all.empty())
        return;

    // Kasus bangkrut ke Bank: triggerPlayer == nullptr
    // Gunakan pemain aktif pertama sebagai initiator agar urutan tetap valid;
    // AuctionManager::setupAuction() akan menyusun urutan mulai dari setelahnya.
    Player *initiator = triggerPlayer ? triggerPlayer : all.front();

    // Logika urutan peserta sepenuhnya diserahkan ke AuctionManager::setupAuction()
    am->setupAuction(prop, initiator, all);
    state.setPhase(GamePhase::AUCTION);

    // Proses lelang akan berjalan di GUI

    log("SYSTEM", "AUCTION_START",
        "Lelang dimulai untuk " + prop->getName());
}

// ─────────────────────────────────────────────
//  Kebangkrutan
// ─────────────────────────────────────────────

int GameMaster::handleDebtPayment(Player *debtor, int debt, Player *creditor)
{
    if (!debtor)
        return -1;

    int cash = debtor->getBalance();
    if (cash >= debt)
    {
        // Cukup bayar langsung
        return 0;
    }

    // Tidak cukup cash → cek potensi likuidasi
    int potential = calculateWealth(debtor);
    state.setPhase(GamePhase::BANKRUPTCY);

    if (potential >= debt)
    {
        // Wajib likuidasi — simpan konteks agar GUI bisa membacanya
        state.setPendingDebt(debt);
        state.setPendingDebtor(debtor);
        state.setPendingCreditor(creditor);
        log(debtor->getUsername(), "BANKRUPTCY_START",
            "Harus likuidasi untuk bayar M" + std::to_string(debt));

        return 1;
    }
    else
    {
        // Tidak bisa bayar sama sekali → langsung bangkrut
        state.setPendingDebt(debt);
        state.setPendingDebtor(debtor);
        state.setPendingCreditor(creditor);
        return 2;
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

    // Salin daftar properti dulu agar removeProperty() tidak invalidate iterator
    std::vector<Property *> props(from->getProperties().begin(),
                                  from->getProperties().end());
    for (Property *p : props)
    {
        if (!p)
            continue;
        p->setOwner(to->getUsername());
        to->addProperty(p);
        from->removeProperty(p);

        log(to->getUsername(), "BELI",
            "Mengambil alih " + p->getName() + " dari " + from->getUsername());
    }

    // Pindahkan semua kartu kemampuan ke creditor
    std::vector<SkillCard *> hand = from->getHand();
    for (SkillCard *c : hand)
    {
        if (c)
        {
            to->forceAddSkillCard(c);
            log(to->getUsername(), "KARTU",
                "Mengambil alih kartu " + c->getType() + " dari " + from->getUsername());
        }
    }
    while (from->getHandSize() > 0)
    {
        from->discardSkillCard(0);
    }

    // Reset pending bankruptcy context
    state.setPendingDebt(0);
    state.setPendingDebtor(nullptr);
    state.setPendingCreditor(nullptr);

    // Tandai bankrut & keluarkan dari urutan giliran
    state.removePlayer(from);

    // Cek apakah permainan selesai
    if (state.countActivePlayers() <= 1)
    {
        state.setPhase(GamePhase::GAME_OVER);
    }
    else
    {
        state.setPhase(GamePhase::PLAYER_TURN);
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
        *from -= remaining;
    }

    // Salin daftar properti dulu agar removeProperty() tidak invalidate iterator
    std::vector<Property *> props(from->getProperties().begin(),
                                  from->getProperties().end());

    // Antrekan semua properti untuk dilelang satu per satu
    state.clearPendingAuctionQueue();
    for (Property *p : props)
    {
        if (!p)
            continue;
        // Hancurkan bangunan jika ada (StreetProperty)
        auto *sp = dynamic_cast<StreetProperty *>(p);
        if (sp)
            sp->resetBuildings();

        p->clearOwner();
        p->setStatus(PropertyStatus::BANK);
        from->removeProperty(p);
        state.addToPendingAuctionQueue(p);

        log("Bank", "BELI",
            p->getName() + " kembali ke Bank dari " + from->getUsername());
    }

    // Kembalikan semua kartu kemampuan ke deck
    CardDeck<SkillCard> *skillDeck = state.getSkillDeck();
    std::vector<SkillCard *> hand = from->getHand();
    for (SkillCard *c : hand)
    {
        if (c && skillDeck)
            skillDeck->discard(c);
    }
    while (from->getHandSize() > 0)
    {
        from->discardSkillCard(0);
    }

    // Reset pending bankruptcy context
    state.setPendingDebt(0);
    state.setPendingDebtor(nullptr);
    state.setPendingCreditor(nullptr);

    // Tandai bankrut & keluarkan dari urutan giliran
    state.removePlayer(from);

    // Mulai lelang properti pertama (jika ada)
    Property *first = state.popPendingAuction();
    if (first)
    {
        startAuction(first, nullptr);
    }
    else if (state.countActivePlayers() <= 1)
    {
        state.setPhase(GamePhase::GAME_OVER);
    }
    else
    {
        state.setPhase(GamePhase::PLAYER_TURN);
    }
}

// ─────────────────────────────────────────────
//  Helper
// ─────────────────────────────────────────────

void GameMaster::sellPropertyToBank(Player *player, Property *prop)
{
    if (!player || !prop)
        return;
    if (prop->getOwnerId() != player->getUsername())
        return;
    if (prop->getStatus() == PropertyStatus::MORTGAGED)
        return;

    int value;
    auto *sp = dynamic_cast<StreetProperty *>(prop);

    if (sp)
    {
        value = sp->calculateSellPrice();
        sp->resetBuildings();
    }
    else
    {
        value = prop->calculateSellPrice();
    }

    prop->clearOwner();
    prop->setStatus(PropertyStatus::BANK);
    player->removeProperty(prop);
    *player += value;

    log(player->getUsername(), "SELL_PROPERTY",
        prop->getName() + " dijual ke bank seharga M" + std::to_string(value));
}

void GameMaster::mortgageProperty(Player *player, Property *prop)
{
    if (!player || !prop)
        return;
    if (prop->getOwnerId() != player->getUsername())
        return;
    if (prop->getStatus() == PropertyStatus::MORTGAGED)
        return;

    auto *sp = dynamic_cast<StreetProperty *>(prop);
    if (sp && sp->getBuildingCount() > 0)
        return;

    int mortgageValue = prop->getMortageValue();
    prop->setStatus(PropertyStatus::MORTGAGED);
    *player += mortgageValue;

    log(player->getUsername(), "MORTGAGE",
        prop->getName() + " digadaikan, menerima M" + std::to_string(mortgageValue));
}

// ─────────────────────────────────────────────
//  processNextCardPayment
//  Proses satu entri dari pendingPaymentQueue.
//  Dipanggil oleh ElectionCard/BirthdayCard::execute() dan
//  oleh BankruptcyDialog setiap kali satu pembayaran selesai.
// ─────────────────────────────────────────────
void GameMaster::processNextCardPayment()
{
    while (state.hasPendingPayment())
    {
        GameState::PendingPayment pay = state.popPendingPayment();

        Player *debtor = pay.debtor;
        Player *creditor = pay.creditor;
        int amount = pay.amount;

        if (!debtor)
            continue;

        // Skip pemain yang sudah bangkrut
        if (debtor->getStatus() == PlayerStatus::BANKRUPT)
            continue;

        if (debtor->getBalance() >= amount)
        {
            // Bayar langsung
            *debtor -= amount;
            if (creditor)
                *creditor += amount;
            log(debtor->getUsername(), "CARD_PAYMENT",
                "Bayar M" + std::to_string(amount) +
                    " ke " + (creditor ? creditor->getUsername() : "Bank"));
        }
        else
        {
            // Tidak cukup → trigger bankruptcy dialog.
            // Kembalikan entri ini ke antrean agar tidak hilang —
            // BankruptcyDialog akan memanggilnya lagi setelah selesai.
            // (Tidak perlu: handleDebtPayment menyimpan debt+creditor ke GameState)
            int status = handleDebtPayment(debtor, amount, creditor);
            if (status == 1)
            {
                // Menunggu dialog likuidasi — stop di sini.
                // BankruptcyDialog::onFinish() akan memanggil processNextCardPayment()
                return;
            }
            else if (status == 2)
            {
                // status == 2: langsung bangkrut, lanjut ke entri berikutnya
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
    }

    // Antrian habis: pastikan fase kembali normal
    if (state.getPhase() == GamePhase::BANKRUPTCY)
        state.setPhase(GamePhase::PLAYER_TURN);
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
        if (!p || p->getStatus() == PropertyStatus::MORTGAGED)
            continue;
        wealth += p->calculateSellPrice();
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
    CardDeck<SkillCard> *deck = state.getSkillDeck();

    if (deck == nullptr)
        return;

    Player *player = state.getCurrPlayer();

    if (player == nullptr || player->getStatus() == PlayerStatus::BANKRUPT)
        return;

    SkillCard *skill = deck->draw();

    if (skill == nullptr)
        return;

    giveSkillCardToPlayer(player, skill);
}

void GameMaster::tickFestivalDurations()
{
    for (Player *p : state.getActivePlayers())
    {
        for (int i = 0; i < p->getPropertyCount(); i++)
        {
            Property *prop = p->getProperties()[i];
            if (!prop)
                continue;

            auto *sp = dynamic_cast<StreetProperty *>(prop);
            if (sp)
            {
                sp->decrementFestivalDuration();
            }
        }
    }
}

void GameMaster::tickDiscountDurations()
{
    for (Player *p : state.getActivePlayers())
    {
        if (!p || !p->hasActiveDiscount())
            continue;

        // Cari DiscountCard yang masih aktif di tangan pemain
        // dan kurangi durasinya. Jika habis, clear discount.
        bool stillActive = false;
        for (SkillCard *sc : p->getHand())
        {
            DiscountCard *dc = dynamic_cast<DiscountCard *>(sc);
            if (!dc)
                continue;
            dc->decreaseDuration();
            if (dc->getDuration() > 0)
                stillActive = true;
        }

        if (!stillActive)
        {
            p->clearDiscount();
            log(p->getUsername(), "DISCOUNT_EXPIRED",
                "Efek diskon habis.");
        }
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

// ─────────────────────────────────────────────
//  Proxy metode dialog GUI
// ─────────────────────────────────────────────

void GameMaster::handleGadai(Property *prop)
{
    if (!prop)
        return;
    if (prop->getStatus() == PropertyStatus::OWNED)
    {
        prop->setStatus(PropertyStatus::MORTGAGED);
        int gadaiVal = prop->getPurchasePrice() / 2; // Asumsi gadai setengah harga
        Player *cur = state.getCurrPlayer();
        *cur += gadaiVal;
        log(cur->getUsername(), "GADAI", "Menggadaikan " + prop->getName() + " seharga M" + std::to_string(gadaiVal));
    }
}

void GameMaster::handleTebus(Property *prop)
{
    if (!prop)
        return;
    if (prop->getStatus() == PropertyStatus::MORTGAGED)
    {
        int tebusVal = prop->getPurchasePrice(); // Asumsi harga tebus sama dengan harga beli penuh
        Player *cur = state.getCurrPlayer();
        if (cur->getBalance() >= tebusVal)
        {
            *cur -= tebusVal;
            prop->setStatus(PropertyStatus::OWNED);
            log(cur->getUsername(), "TEBUS", "Menebus " + prop->getName() + " seharga M" + std::to_string(tebusVal));
        }
    }
}

void GameMaster::handleBangun(StreetProperty *sp)
{
    if (!sp)
        return;
    Player *cur = state.getCurrPlayer();
    int cost = sp->getHouseUpgCost();
    if (cur->getBalance() >= cost && !sp->gethasHotel())
    {
        *cur -= cost;
        // Asumsikan build function, ini placeholder. Bisa diimplementasikan spesifik nanti oleh anggota lain.
        log(cur->getUsername(), "BANGUN", "Membangun di " + sp->getName() + " seharga M" + std::to_string(cost));
    }
}

void GameMaster::handleJualBangunan(StreetProperty *sp)
{
    if (!sp)
        return;
    Player *cur = state.getCurrPlayer();
    if (sp->getBuildingCount() > 0 || sp->gethasHotel())
    {
        int sellVal = sp->getHouseUpgCost() / 2;
        // Asumsikan sell function placeholder.
        *cur += sellVal;
        log(cur->getUsername(), "JUAL_BANGUNAN", "Menjual bangunan di " + sp->getName() + " seharga M" + std::to_string(sellVal));
    }
}

void GameMaster::handleSkillCardOverflow(Player *player)
{
    if (player == nullptr)
        return;

    if (player->getHandSize() > 3)
    {
        state.setPendingSkillDrop(
            player,
            "Kamu memiliki lebih dari 3 kartu kemampuan. Pilih 1 kartu untuk dibuang.");

        state.setPhase(GamePhase::AWAITING_DROP_SKILL_CARD);
    }
}

void GameMaster::giveSkillCardToPlayer(Player *player, SkillCard *card)
{
    if (player == nullptr || card == nullptr)
        return;

    player->forceAddSkillCard(card);

    log(
        player->getUsername(),
        "DRAW_SKILL_CARD",
        "Mendapat kartu kemampuan: " + card->getType());

    if (player->getHandSize() > 3)
    {
        handleSkillCardOverflow(player);
    }
}

void GameMaster::useSkillCard(Player *player, SkillCard *card, GameState &gs)
{
    if (!player || !card)
        throw runtime_error("Player atau kartu tidak valid.");

    if (gs.getHasUsedCard())
        throw runtime_error("Kamu sudah menggunakan kartu kemampuan pada giliran ini.");

    if (card->isUsed())
        throw runtime_error("Kartu ini sudah digunakan.");

    card->execute(*player, gs);

    if (gs.getLogger())
    {
        gs.getLogger()->addLog(
            gs.getCurrTurn(),
            player->getUsername(),
            "SKILL_CARD",
            card->getDescription());
    }

    if (!card->isUsed())
        card->markUsed();

    const vector<SkillCard *> &hand = player->getHand();

    for (int i = 0; i < static_cast<int>(hand.size()); i++)
    {
        if (hand[i] == card)
        {
            SkillCard *removed = player->discardSkillCard(i);

            if (removed != nullptr && state.getSkillDeck() != nullptr)
            {
                state.getSkillDeck()->discard(removed);
            }

            break;
        }
    }

    player->markCardUsedThisTurn();
    gs.setHasUsedCard(true);
}