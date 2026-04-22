#include "Board.hpp"

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

string tileNameToString(TileName name)
{
    switch (name)
    {
    case TileName::GO:
        return "GO";
    case TileName::GARUT:
        return "GARUT";
    case TileName::COMMON_FUND:
        return "DANA_UMUM";
    case TileName::TASIKMALAYA:
        return "TASIKMALAYA";
    case TileName::INCOME_TAX:
        return "PAJAK_PENGHASILAN";
    case TileName::ST_GAMBIR:
        return "STASIUN_GAMBIR";
    case TileName::BOGOR:
        return "BOGOR";
    case TileName::FESTIVAL:
        return "FESTIVAL";
    case TileName::DEPOK:
        return "DEPOK";
    case TileName::BEKASI:
        return "BEKASI";
    case TileName::JAIL:
        return "PENJARA";
    case TileName::MAGELANG:
        return "MAGELANG";
    case TileName::PLN:
        return "PLN";
    case TileName::SOLO:
        return "SOLO";
    case TileName::YOGYAKARTA:
        return "YOGYAKARTA";
    case TileName::ST_BANDUNG:
        return "STASIUN_BANDUNG";
    case TileName::MALANG:
        return "MALANG";
    case TileName::SEMARANG:
        return "SEMARANG";
    case TileName::SURABAYA:
        return "SURABAYA";
    case TileName::FREE_PARK:
        return "BEBAS_PARKIR";
    case TileName::MAKASSAR:
        return "MAKASSAR";
    case TileName::CHANCE:
        return "KESEMPATAN";
    case TileName::BALIKPAPAN:
        return "BALIKPAPAN";
    case TileName::MANADO:
        return "MANADO";
    case TileName::ST_TUGU:
        return "STASIUN_TUGU";
    case TileName::PALEMBANG:
        return "PALEMBANG";
    case TileName::PEKANBARU:
        return "PEKANBARU";
    case TileName::PAM:
        return "PAM";
    case TileName::MEDAN:
        return "MEDAN";
    case TileName::GO_TO_JAIL:
        return "PERGI_KE_PENJARA";
    case TileName::BANDUNG:
        return "BANDUNG";
    case TileName::DENPASAR:
        return "DENPASAR";
    case TileName::MATARAM:
        return "MATARAM";
    case TileName::ST_GUBENG:
        return "STASIUN_GUBENG";
    case TileName::JAKARTA:
        return "JAKARTA";
    case TileName::LUX_GOODS:
        return "PAJAK_BARANG_MEWAH";
    case TileName::IKN:
        return "IBU_KOTA_NUSANTARA";
    default:
        return "UNKNOWN";
    }
}

Board::Board(const vector<Tile *> &Tiles, int Size): tiles(Tiles), size(Size){}
Board::~Board(){
    for(Tile* t: tiles){
        delete t;
    }
    tiles.clear();
}

Tile *Board::getTile(int idx) const
{
    if (idx < 0 || idx >= size)
        return nullptr;
    return tiles[idx];
}
int Board::getNextTileIndex(int cur, int steps) const
{
    if (size == 0)
        return 0;
    return (cur + steps) % size;
}
Tile *Board::getNextTile(int cur, int steps) const
{
    return getTile(getNextTileIndex(cur, steps));
}
int Board::getSize() const{return size;}
int Board::findTileIndexByCode(const string &code) const
{
    for (int i = 0; i < size; ++i)
    {
        if (tiles[i] && tiles[i]->getCode() == code)
        {
            return i;
        }
    }
    return -1; 
}
Tile *Board::findTileByCode(const string &code) const
{
    int idx = findTileIndexByCode(code);
    if (idx == -1)
        return nullptr;
    return tiles[idx];
}

Tile::Tile(int id, string display, TileType type, TileName name, string code)
    : id(id), colorDisplay(display), type(type), name(name), code(code) {}
int Tile::getIndex() const {return id;}
string Tile::getCode() const{return code;}
string Tile::getTileName() const
{
    return tileNameToString(name);
}

string Tile::getTileType() const
{
    return tileTypeToString(type);
}
string Tile::getDisp() const { return colorDisplay; }


ActionTile::ActionTile(int id, string display, TileType type, TileName name, string code):Tile(id, display, type, name, code){}

PropertyTile::PropertyTile(int id, string display, TileType type, TileName name, string code, Property* prop) : Tile(id, display, type, name, code), prop(prop){}
Property* PropertyTile::getProperty() const {return prop;}
void PropertyTile::onLanded(Player &p, GameState &gs)
{
    if (!prop)
        return;
    
    if(prop->getStatus() == "MORTGAGED") return;

    if (prop->getStatus() == "OWNED" && prop->getOwner() != &p)
    {
        return handleOwnedByOther(p, gs);
    }

    if(prop->getStatus() == "BANK") return handleUnowned(p, gs);

}

StreetTile::StreetTile(int id, string display, TileType type, TileName name, string code, Property* prop) : PropertyTile(id, display, type, name, code, prop) {}
int StreetTile::calculateRent(int diceTotal) const{
    if (!prop)
        return 0;
    StreetProperty *sp = dynamic_cast<StreetProperty*>(prop);
    if (!sp)
        return 0;
    return static_cast<int>(sp->calculateRentPrice(false));
}
void StreetTile::handleOwnedByOther(Player& p, GameState& gs){
    // BayarSewaCommand cmdSewa(p, this, gs.getGameBank());
    // cmdSewa.execute(gs.getGameMaster());
    (void) p; (void) gs;
}
void StreetTile::handleUnowned(Player& p, GameState& gs){
    // BeliCommand cmdBeli(p, this, gs.getGameBank());
    // cmdBeli.execute(gs.getGameMaster());

    (void)p;
    (void)gs;
}

RailroadTile::RailroadTile(int id, string display, TileType type, TileName name, string code, Property* prop) : PropertyTile(id, display, type, name, code, prop) {}
int RailroadTile::calculateRent(int diceTotal) const{
    if (!prop)
        return 0;
    RailroadProperty *rp = dynamic_cast<RailroadProperty *>(prop);
    if (!rp)
        return 0;
    return static_cast<int>(rp->calculateRentPrice());
}
void RailroadTile::handleUnowned(Player &p, GameState &gs)
{
    // BeliCommand cmdBeli(p, this, gs.getGameBank());
    // cmdBeli.execute(gs.getGameMaster());
    (void)p;
    (void)gs;
}
void RailroadTile::handleOwnedByOther(Player &p, GameState &gs)
{
    // BayarSewaCommand cmdSewa(p, this, gs.getGameBank());
    // cmdSewa.execute(gs.getGameMaster());
    (void)p;
    (void)gs;
}

UtilityTile::UtilityTile(int id, string display, TileType type, TileName name, string code, Property* prop) :PropertyTile(id, display, type, name, code, prop) {}
int UtilityTile::calculateRent(int diceTotal)const {
    if (!prop)
        return 0;
    UtilityProperty *up = dynamic_cast<UtilityProperty *>(prop);
    if (!up)
        return 0;
    return static_cast<int>(up->calculateRentPrice()) * diceTotal;
   
}
void UtilityTile::handleUnowned(Player &p, GameState &gs)
{
    // BeliCommand cmdBeli(p, this, gs.getGameBank());
    // cmdBeli.execute(gs.getGameMaster());
    (void) p; (void)gs;
}
void UtilityTile::handleOwnedByOther(Player &p, GameState &gs){
    // BayarSewaCommand cmdSewa(p, this, gs.getGameBank());
    // cmdSewa.execute(gs.getGameMaster());
    (void)p;
    (void)gs;
}

GoTile::GoTile(int id, string display, TileType type, TileName name, string code, int salary) : ActionTile(id, display, type, name, code), salary(salary){}
void GoTile::onLanded(Player& p, GameState& gs) {
    p + salary;
    (void)gs;
}
void GoTile::onPassed(Player& p, GameState& gs){
    p + salary;
    (void) gs;
}
int GoTile::getSalary() const{
    return salary;
}

JailTile::JailTile(int id, string display, TileType type, TileName name, string code,
                   int jailFine) : ActionTile(id, display, type, name, code), jailFine(jailFine), jailIndex(id) {}
int JailTile::getJailFine() const{return jailFine;}
void JailTile::onLanded(Player& p, GameState& gs) {
    visitors.push_back(&p);
    (void) gs;
}
void JailTile::sendToJail(Player& p){
    visitors.erase(
        remove(visitors.begin(), visitors.end(), &p),
        visitors.end());
    if (!isInmate(p))
    {
        inmates.push_back(&p);
    }
    p.setStatus("JAILED");
    p.moveTo(jailIndex);
}
bool JailTile::tryEscape(Player& p, Dice& d){
    d.rollRandom();
    if (d.getConsecutiveDoubles() > 0)
    {
        release(p);
        return true;
    }
    return false;
}

void JailTile::payFine(Player& p, Bank& b){
    p - jailFine;
    b.receiveFromPlayer(p, jailFine);
    release(p);
}
void JailTile::useJailCard(Player& p){
    // Kartu bebas penjara ada ketidaksamaan di spek. Pada JailTile dibilang ada tetapi saat di spek card tidak ada jenis tsb
    release(p);
}
void JailTile::release(Player& p){
    inmates.erase(std::remove(inmates.begin(), inmates.end(), &p), inmates.end());
    p.setStatus("ACTIVE");
}
bool JailTile::isInmate(const Player& p) const{
    for (const Player* inmate : inmates)
    {
        if (inmate->getID() == p.getID())
            return true;
    }
    return false;   
}

GoToJailTile::GoToJailTile(int id, string display, TileType type, TileName name, string code) : ActionTile(id, display, type, name, code) {}
void GoToJailTile::onLanded(Player& p, GameState& gs) {
    Board *board = gs.getGameBoard();
    Tile *jailTilePtr = board->findTileByCode("PEN");

    JailTile *jail = dynamic_cast<JailTile *>(jailTilePtr);
    if (!jail)
        return;

    int jailIdx = jail->getIndex();
    p.moveTo(jailIdx);

    jail->sendToJail(p);
}

FreeParkingTile::FreeParkingTile(int id, string display, TileType type, TileName name, string code): ActionTile(id, display, type, name, code){}
void FreeParkingTile::onLanded(Player& p, GameState& gs) {
    (void)p;
    (void)gs;
}

TaxTile::TaxTile(int id, string display, TileType type, TileName name, string code) : ActionTile(id, display, type, name, code) {}
void TaxTile::onLanded(Player& p, GameState& gs) {

    // BayarPajakCommand cmdPajak(p, this, gs.getGameBank());
    // cmdPajak.execute();
    (void)p;
    (void)gs;
}

CardTile::CardTile(int id, string display, TileType type, TileName name, string code, CardDeck* cardDeck) : ActionTile(id, display, type, name, code), card(cardDeck){}
void CardTile::onLanded(Player& p, GameState& gs) {
    if (!deck)
        return;

    Card *kartu = deck->draw();
    if (!kartu)
        return;

    kartu->execute(p, gs.getGameMaster());

  
    deck->discard(kartu);
}

FestivalTile::FestivalTile(int id, string display, TileType type, TileName name, string code) : ActionTile(id, display, type, name, code) {}
void FestivalTile::onLanded(Player &, GameState &) {
    // Di laporan tidak ada yang handle ini, apakah harus dari kelas ini sendiri?
    // FestivalCommand cmdFestival(p, gs);
    // cmdFestival.execute(gs.getGameMaster());
    (void)p;
    (void)gs;
}