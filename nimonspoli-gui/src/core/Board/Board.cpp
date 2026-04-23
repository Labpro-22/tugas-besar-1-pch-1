#include "Board.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"
#include "../Player/Player.hpp"

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

// FIX #2: const di akhir, bukan sebelum parameter
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

// FIX #3: `tiles` → `tile`
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
string Tile::getPic() const {return picDisplay;}

// ═════════════════════════════════════════════
//  ActionTile
// ═════════════════════════════════════════════

// FIX #4: tileName → TileName (kapital T)
ActionTile::ActionTile(int id, string display, TileType type, string name, string code)
    : Tile(id, display, type, name, code) {}

// ═════════════════════════════════════════════
//  PropertyTile
// ═════════════════════════════════════════════

// FIX #5 #6: Board.hpp tidak ada prop di constructor → prop diinit nullptr
// FIX #7: virtual int calculateRent = 0 di luar class dihapus
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
        // BeliCommand dipanggil dari dispatcher berdasarkan phase
        gs.setPhase(GamePhase::AWAITING_BUY);
    }
    else if (property->getStatus() == PropertyStatus::OWNED)
    {
        if (property->getOwnerId() != p.getUsername())
        {
            // BayarSewaCommand dipanggil dari dispatcher
            (void)gs;
        }
    }
    // MORTGAGED → tidak ada aksi
}

// FIX #10: tambah const; akses prop via getProperty() bukan direct
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

// FIX #8: RailRoadTile → RailroadTile
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
    }
    else if (property->getStatus() == PropertyStatus::OWNED)
    {
        if (property->getOwnerId() != p.getUsername())
        {
            (void)gs; // BayarSewaCommand handle dari dispatcher
        }
    }
}

// FIX #8: tambah class qualifier RailroadTile:: dan const
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

// FIX #9: tambah `:` sebelum PropertyTile
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
    }
    else if (property->getStatus() == PropertyStatus::OWNED)
    {
        if (property->getOwnerId() != p.getUsername())
        {
            (void)gs;
        }
    }
}

// FIX #10: tambah const
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

// FIX #11: `p + salary` tidak mengubah uang → harus via Bank::payPlayer
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

// FIX #12: push_back(p) → push_back(&p) karena vector<Player*>
void JailTile::onLanded(Player &p, GameState & /*gs*/)
{
    if (!isInmate(p))
    {
        visitor.push_back(&p);
    }
}

void JailTile::sendToJail(Player &p)
{
    visitor.erase(remove(visitor.begin(), visitor.end(), &p), visitor.end());
    if (!isInmate(p))
    {
        inmates.push_back(&p);
    }
    p.setStatus(PlayerStatus::JAILED);
}

// FIX #13: getConsecutiveDoubles() → isDouble() untuk cek hasil lemparan ini
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

// FIX #14: Bank:: Player butuh Player*, bukan Player&
void JailTile::payFine(Player &p, Bank &b)
{
    if (p.getBalance() < jailFine) {
        p.setStatus(PlayerStatus::BANKRUPT);
        // Handle Bankrupt by Game Master
    }
    p -= jailFine;
    // Cek apakah player akan Bankrupt? <-- perlu dihandle?
    
    release(p);
}

void JailTile::useJailCard(Player &p)
{
    // Catatan: kartu "Bebas dari Penjara" tidak ada di spesifikasi kartu kemampuan.
    // Method ini tetap ada untuk handle edge case jika ditambahkan nanti.
    release(p);
}

void JailTile::release(Player &p)
{
    inmates.erase(remove(inmates.begin(), inmates.end(), &p), inmates.end());
    visitor.erase(remove(visitor.begin(), visitor.end(), &p), visitor.end());
    p.setStatus(PlayerStatus::ACTIVE);
}

// FIX #15: tambah const sesuai deklarasi di header
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
    // Gunakan GameMaster::sendPlayerToJail() agar semua side-effect tertangani:
    // posisi, status JAILED, log, dan endCurrentTurn
    GameMaster *gm = gs.getGameMaster();
    if (gm)
    {
        gm->sendPlayerToJail(&p);
        return;
    }

    // Fallback jika GameMaster belum terdaftar (tidak seharusnya terjadi)
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

// FIX #17: BayarPajakCommand tidak di-instantiate di sini
// onLanded hanya set fase; Command dispatcher yang panggil BayarPajakCommand
void TaxTile::onLanded(Player & /*p*/, GameState &gs)
{
    gs.setPhase(GamePhase::PLAYER_TURN);
}

// ═════════════════════════════════════════════
//  CardTile
// ═════════════════════════════════════════════

// FIX #18: CardDeck* → CardDeck<Card>*
CardTile::CardTile(int id, string display, TileType type, string name, string code,
                   CardDeck<Card> *cardDeck)
    : ActionTile(id, display, type, name, code), card(cardDeck) {}

// FIX #18: `deck` → `card` (nama atribut di header)
// FIX #20: gs.getGameMaster() tidak ada di GameState API
// TODO: Card::execute(p, gm) perlu GameMaster — tambahkan setelah GameState menyimpan ref ke GameMaster
void CardTile::onLanded(Player &p, GameState &gs)
{
    if (!card)
        return;

    Card *kartu = card->draw();
    if (!kartu)
        return;

    kartu->execute(p, gs);

    card->discard(kartu);
}

// ═════════════════════════════════════════════
//  FestivalTile
// ═════════════════════════════════════════════

FestivalTile::FestivalTile(int id, string display, TileType type, string name, string code)
    : ActionTile(id, display, type, name, code) {}

// FIX #19: parameter anonymous diberi nama; FestivalCommand dihandle dispatcher
void FestivalTile::onLanded(Player & /*p*/, GameState &gs)
{
    // TODO: FestivalCommand dipanggil dari dispatcher
    gs.setPhase(GamePhase::PLAYER_TURN);
}