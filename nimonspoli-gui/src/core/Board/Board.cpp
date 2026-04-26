#include "Board.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"
#include "../Player/Player.hpp"
#include "../utils/TransactionLogger.hpp"

// ═════════════════════════════════════════════
//  Helper functions
// ═════════════════════════════════════════════

string tileTypeToString(TileType type)
{
    switch (type)
    {
    case TileType::SPECIAL:
        return "SPECIAL";
    case TileType::STREET:
        return "STREET";
    case TileType::CARD:
        return "CARD";
    case TileType::TAX:
        return "TAX";
    case TileType::RAILROAD:
        return "RAILROAD";
    case TileType::FESTIVAL:
        return "FESTIVAL";
    case TileType::UTILITY:
        return "UTILITY";
    default:
        return "UNKNOWN";
    }
}

// ═════════════════════════════════════════════
//  Board
// ═════════════════════════════════════════════

Board::Board(const vector<Tile *> &Tile, int Size)
    : tile(Tile), size(Size) {}

Board::~Board()
{
    for (Tile *t : tile)
    {
        delete t;
    }
}

Tile *Board::getTile(int idx) const
{
    if (idx < 0 || idx >= size)
        return nullptr;
    return tile[idx];
}

Tile *Board::getNextTile(int cur, int next) const
{
    int nextIdx = (cur + next) % size;
    return tile[nextIdx];
}

int Board::getSize() const { return size; }

int Board::findTileIndexByCode(const string &code) const
{
    for (int i = 0; i < size; ++i)
    {
        if (tile[i] && tile[i]->getCode() == code)
            return i;
    }
    return -1;
}

Tile *Board::findTileByCode(const string &code) const
{
    int idx = findTileIndexByCode(code);
    if (idx == -1)
        return nullptr;
    return tile[idx];
}

Property *Board::findPropertyById(int id) const
{
    for (Tile *t : tile)
    {
        PropertyTile *pt = dynamic_cast<PropertyTile *>(t);
        if (!pt)
            continue;

        Property *prop = pt->getProperty();
        if (prop && prop->getId() == id)
            return prop;
    }

    return nullptr;
}

// ═════════════════════════════════════════════
//  Tile (base)
// ═════════════════════════════════════════════

Tile::Tile(int id, string display, TileType type, string name, string code)
    : id(id), colorDisplay(display), type(type), name(name), code(code) {}

int Tile::getIndex() const { return id; }
string Tile::getCode() const { return code; }
string Tile::getTileName() const { return name; }
string Tile::getTileType() const { return tileTypeToString(type); }
string Tile::getDisp() const { return colorDisplay; }
string Tile::getPic() const { return picDisplay; }

// ═════════════════════════════════════════════
//  ActionTile
// ═════════════════════════════════════════════

ActionTile::ActionTile(int id, string display, TileType type, string name, string code)
    : Tile(id, display, type, name, code) {}

// ═════════════════════════════════════════════
//  PropertyTile
// ═════════════════════════════════════════════

PropertyTile::PropertyTile(int id, string display, TileType type, string name, string code)
    : Tile(id, display, type, name, code), prop(nullptr) {}

Property *PropertyTile::getProperty() const { return prop; }

// ═════════════════════════════════════════════
//  StreetTile
// ═════════════════════════════════════════════

StreetTile::StreetTile(int id, string display, TileType type, string name, string code)
    : PropertyTile(id, display, type, name, code) {}

void StreetTile::onLanded(Player &p, GameState &gs)
{
    Property *property = getProperty();
    if (!property)
        return;

    if (property->getStatus() == PropertyStatus::BANK)
    {
        gs.setPhase(GamePhase::AWAITING_BUY);
    }
    else if (property->getStatus() == PropertyStatus::OWNED)
    {
        if (property->getOwnerId() != p.getUsername())
        {
            if (!p.isShielded())
            {
                // Simpan data sewa ke GameState, biarkan dialog yang eksekusi
                gs.setPendingRentTile(this);
                gs.setPendingRentDice(gs.getDice() ? gs.getDice()->getTotal() : 0);
                gs.setPhase(GamePhase::AWAITING_RENT);
            }
        }
    }
    // MORTGAGED → tidak ada aksi
}

int StreetTile::calculateRent(int /*diceTotal*/) const
{
    Property *property = getProperty();
    if (!property)
        return 0;
    StreetProperty *sp = dynamic_cast<StreetProperty *>(property);
    if (!sp)
        return 0;
    return sp->calculateRentPrice(0, 1, false);
}

// ═════════════════════════════════════════════
//  RailroadTile
// ═════════════════════════════════════════════

RailRoadTile::RailRoadTile(int id, string display, TileType type, string name, string code)
    : PropertyTile(id, display, type, name, code) {}

void RailRoadTile::onLanded(Player &p, GameState &gs)
{
    Property *property = getProperty();
    if (!property)
        return;

    if (property->getStatus() == PropertyStatus::BANK)
    {
        // Railroad: gratis, langsung milik pemain pertama
        property->setOwner(p.getUsername());
        property->setStatus(PropertyStatus::OWNED);
        p.addProperty(property);

        // Log ke TransactionLogger
        TransactionLogger *logger = gs.getLogger();
        if (logger)
        {
            logger->addLog(gs.getCurrTurn(), p.getUsername(), "BELI",
                           "Akuisisi otomatis " + property->getName() + " (Gratis)");
        }
    }
    else if (property->getStatus() == PropertyStatus::OWNED)
    {
        if (property->getOwnerId() != p.getUsername())
        {
            if (!p.isShielded())
            {
                gs.setPendingRentTile(this);
                gs.setPendingRentDice(gs.getDice() ? gs.getDice()->getTotal() : 0);
                gs.setPhase(GamePhase::AWAITING_RENT);
            }
        }
    }
}

int RailRoadTile::calculateRent(int /*diceTotal*/) const
{
    Property *property = getProperty();
    if (!property)
        return 0;
    RailroadProperty *rp = dynamic_cast<RailroadProperty *>(property);
    if (!rp)
        return 0;
    return rp->calculateRentPrice(0, 1, false);
}

// ═════════════════════════════════════════════
//  UtilityTile
// ═════════════════════════════════════════════

UtilityTile::UtilityTile(int id, string display, TileType type, string name, string code)
    : PropertyTile(id, display, type, name, code) {}

void UtilityTile::onLanded(Player &p, GameState &gs)
{
    Property *property = getProperty();
    if (!property)
        return;

    if (property->getStatus() == PropertyStatus::BANK)
    {
        // Utility: gratis, langsung milik pemain pertama
        property->setOwner(p.getUsername());
        property->setStatus(PropertyStatus::OWNED);
        p.addProperty(property);

        // Log ke TransactionLogger
        TransactionLogger *logger = gs.getLogger();
        if (logger)
        {
            logger->addLog(gs.getCurrTurn(), p.getUsername(), "BELI",
                           "Akuisisi otomatis " + property->getName() + " (Gratis)");
        }
    }
    else if (property->getStatus() == PropertyStatus::OWNED)
    {
        if (property->getOwnerId() != p.getUsername())
        {
            if (!p.isShielded())
            {
                gs.setPendingRentTile(this);
                gs.setPendingRentDice(gs.getDice() ? gs.getDice()->getTotal() : 0);
                gs.setPhase(GamePhase::AWAITING_RENT);
            }
        }
    }
}

int UtilityTile::calculateRent(int diceTotal) const
{
    Property *property = getProperty();
    if (!property)
        return 0;
    UtilityProperty *up = dynamic_cast<UtilityProperty *>(property);
    if (!up)
        return 0;
    return up->calculateRentPrice(diceTotal, 1, false);
}

// ═════════════════════════════════════════════
//  GoTile
// ═════════════════════════════════════════════

GoTile::GoTile(int id, string display, TileType type, string name, string code, int salary)
    : ActionTile(id, display, type, name, code), salary(salary) {}

void GoTile::onLanded(Player &p, GameState &gs)
{
    Bank *bank = gs.getBank();
    if (bank)
        bank->payPlayer(&p, salary);
}

void GoTile::onPassed(Player &p, GameState &gs)
{
    Bank *bank = gs.getBank();
    if (bank)
        bank->payPlayer(&p, salary);
}

int GoTile::getSalary() const { return salary; }

// ═════════════════════════════════════════════
//  JailTile
// ═════════════════════════════════════════════

JailTile::JailTile(int id, string display, TileType type, string name, string code,
                   vector<Player *> inmates, vector<Player *> visitor, int jailFine)
    : ActionTile(id, display, type, name, code),
      inmates(inmates), visitor(visitor), jailFine(jailFine) {}

int JailTile::getJailFine() const { return jailFine; }

void JailTile::onLanded(Player &p, GameState & /*gs*/)
{
    if (!isInmate(p))
    {
        visitor.push_back(&p);
    }
}

void JailTile::sendToJail(Player &p)
{
    if(p.isShielded()) return;
    else{
        visitor.erase(remove(visitor.begin(), visitor.end(), &p), visitor.end());
        if (!isInmate(p))
        {
            inmates.push_back(&p);
        }
        p.setStatus(PlayerStatus::JAILED);
    }
  
}

bool JailTile::tryEscape(Player &p, Dice &d)
{
    d.rollRandom();
    if (d.isDouble())
    {
        release(p);
        return true;
    }
    return false;
}

void JailTile::payFine(Player &p, GameState &gs)
{
    GameMaster *gm = gs.getGameMaster();
    if (p.getBalance() >= jailFine)
    {
        p -= jailFine;
        release(p);
    }
    else
    {
        if (gm)
        {
            int status = gm->handleDebtPayment(&p, jailFine, nullptr);
            if (status == 2) {
                gm->handleBankruptcy(&p, gs.getBank());
            }
            release(p);
        }
        else
        {
            p.setStatus(PlayerStatus::BANKRUPT);
        }
    }
}

void JailTile::useJailCard(Player &p)
{
    release(p);
}

void JailTile::release(Player &p)
{
    inmates.erase(remove(inmates.begin(), inmates.end(), &p), inmates.end());
    visitor.erase(remove(visitor.begin(), visitor.end(), &p), visitor.end());
    p.setStatus(PlayerStatus::ACTIVE);
}

bool JailTile::isInmate(Player &p) const
{
    for (auto inmate : inmates)
    {
        if (inmate->getUsername() == p.getUsername())
            return true;
    }
    return false;
}

// ═════════════════════════════════════════════
//  GoToJail
// ═════════════════════════════════════════════

GoToJail::GoToJail(int id, string display, TileType type, string name, string code)
    : ActionTile(id, display, type, name, code) {}

void GoToJail::onLanded(Player &p, GameState &gs)
{
    GameMaster *gm = gs.getGameMaster();
    if (gm)
    {
        gm->sendPlayerToJail(&p);
        return;
    }

    Board *board = gs.getBoard();
    if (!board)
        return;
    int jailIdx = board->findTileIndexByCode("PEN");
    if (jailIdx == -1)
        return;
    p.setPosition(jailIdx);
    p.goToJail();
}

// ═════════════════════════════════════════════
//  FreeParkingTile
// ═════════════════════════════════════════════

FreeParkingTile::FreeParkingTile(int id, string display, TileType type, string name, string code)
    : ActionTile(id, display, type, name, code) {}

void FreeParkingTile::onLanded(Player & /*p*/, GameState & /*gs*/)
{
    // Rest area — tidak ada aksi
}

// ═════════════════════════════════════════════
//  TaxTile
// ═════════════════════════════════════════════

TaxTile::TaxTile(int id, string display, TileType type, string name, string code)
    : ActionTile(id, display, type, name, code) {}

// PPH  → set AWAITING_TAX, simpan data ke GameState, dialog yang handle pilihan user.
// PBM  → eksekusi langsung via command, simpan amount hasil, set AWAITING_PBM agar
//         dialog info muncul. User tekan OK → kembali ke PLAYER_TURN.
void TaxTile::onLanded(Player &p, GameState &gs)
{
    TransactionLogger *logger = gs.getLogger();
    int turn = gs.getCurrTurn();
    TaxConfig taxCfg = gs.getTaxConfig();
    Bank *bank = gs.getBank();
    GameMaster *gm = gs.getGameMaster();

    if (code == "PPH")
    {
        // PPH: simpan data ke GameState lalu set phase AWAITING_TAX
        // GUI (TaxDialog) yang akan memanggil handlePPHChoice()
        gs.setPendingPphFlat(static_cast<int>(taxCfg.pphFlat));
        gs.setPendingPphPct(static_cast<int>(taxCfg.pphPercentage));
        gs.setPhase(GamePhase::AWAITING_TAX);
    }
    else // PBM
    {
        // PBM: langsung eksekusi, lalu tampilkan dialog info
        int flatAmount = static_cast<int>(taxCfg.pbmFlat);
        gs.setPendingPbmAmount(flatAmount); // simpan dulu buat display

        if (!gm)
        {
            gs.setPhase(GamePhase::AWAITING_PBM);
            return;
        }

        BayarPajakCommand cmd(&p, this, bank, logger, taxCfg, turn);
        try
        {
            cmd.execute(*gm);
        }
        catch (const InsufficientFundsException &)
        {
            gs.setPhase(GamePhase::BANKRUPTCY);
            return;
        }

        if (p.getStatus() == PlayerStatus::BANKRUPT || gs.getPhase() == GamePhase::BANKRUPTCY)
        {
            return;
        }

        // Set phase AWAITING_PBM agar GUI menampilkan dialog info PBM
        gs.setPhase(GamePhase::AWAITING_PBM);
    }
}

// ═════════════════════════════════════════════
//  CardTile
// ═════════════════════════════════════════════

CardTile::CardTile(int id, string display, TileType type, string name, string code,
                   CardDeck<Card> *cardDeck)
    : ActionTile(id, display, type, name, code), card(cardDeck) {}

void CardTile::onLanded(Player &p, GameState &gs)
{
    if (!card)
        return;

    std::string label = (code == "KSP") ? "Kesempatan" : "Dana Umum";
    TransactionLogger *logger = gs.getLogger();
    int turn = gs.getCurrTurn();
    GameMaster *gm = gs.getGameMaster();

    CardCommand cmd(&p, card, &gs, logger, turn, label);
    if (!gm)
        return;

    try
    {
        cmd.execute(*gm);
    }
    catch (const InsufficientFundsException &)
    {
        gs.setPendingCardDesc(cmd.getLastDescription());
        gs.setPendingCardDeck(label);
        gs.setPhase(GamePhase::BANKRUPTCY);
        return;
    }

    gs.setPendingCardDesc(cmd.getLastDescription());
    gs.setPendingCardDeck(label);

    GamePhase cur = gs.getPhase();
    if (cur == GamePhase::PLAYER_TURN || cur == GamePhase::DICE_ROLLED)
        gs.setPhase(GamePhase::SHOW_CARD);
}
// ═════════════════════════════════════════════
//  FestivalTile
// ═════════════════════════════════════════════

FestivalTile::FestivalTile(int id, string display, TileType type, string name, string code)
    : ActionTile(id, display, type, name, code) {}

// FestivalTile::onLanded: SELALU set phase AWAITING_FESTIVAL.
// GUI (FestivalDialog) menampilkan dialog:
//   - Jika ada properti eligible → tampilkan list untuk dipilih.
//   - Jika tidak ada → tampilkan pesan info, user klik OK untuk lanjut.
void FestivalTile::onLanded(Player &p, GameState &gs)
{
    // Selalu masuk ke phase AWAITING_FESTIVAL agar GUI selalu menampilkan dialog
    // (baik ada properti maupun tidak)
    gs.setPhase(GamePhase::AWAITING_FESTIVAL);

    std::cout << "[DEBUG] FestivalTile: " << p.getUsername()
              << " landed. Phase set to AWAITING_FESTIVAL." << std::endl;
}