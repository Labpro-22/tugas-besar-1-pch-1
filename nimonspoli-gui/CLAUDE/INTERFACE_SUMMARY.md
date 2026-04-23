# INTERFACE SUMMARY — Nimonspoli (Kelompok PCH)

> Method signatures & atribut tiap class berdasarkan Laporan M1.
> Paste hanya bagian yang relevan ke Claude saat sesi coding.

---

## GameMaster

```cpp
class GameMaster {
  GameState gameState;
  TransactionLogger logger;

  GameMaster(GameState gs);
  void start();
  void handleTurn(string cmd);
  void nextTurn();
  bool isCompleted() const;
  void handleWinner(GameState& state);
  void handleBankruptcy(Player* from, Player* to);    // bangkrut ke pemain
  void handleBankruptcy(Player* from, Bank* bank);    // bangkrut ke Bank
};
```

---

## GameState

```cpp
class GameState {
  int currTurn;
  int maxTurn;
  GamePhase phase;
  vector<Player*> listPlayer;
  int currPlayerIdx;
  bool hasExtraTurn;
  bool hasRolled;
  bool hasUsedCard;
  Board* gameBoard;
  Bank* gameBank;
  Dice* gameDice;
  AuctionManager* auctionManager;
  CardDeck<Card>* chanceCardDeck;
  CardDeck<Card>* communityCardDeck;
  CardDeck<Card>* skillCardDeck;
  TransactionLogger* logger;
  GameMaster* gameMaster;   // di-set oleh GameMaster konstruktor

  // Getter turn & fase
  int getCurrTurn() const;
  int getMaxTurn() const;
  GamePhase getPhase() const;
  bool isGameOver() const;
  bool isMaxTurnReached() const;

  // Getter pemain
  Player* getCurrPlayer() const;
  int getCurrPlayerIdx() const;
  vector<Player*> getPlayers() const;
  vector<Player*> getActivePlayers() const;
  Player* getPlayerById(const string& id) const;
  int getPlayerCount() const;
  int countActivePlayers() const;
  bool getHasExtraTurn() const;
  bool getHasRolled() const;
  bool getHasUsedCard() const;

  // Getter entitas
  Board* getBoard() const;
  Bank* getBank() const;
  Dice* getDice() const;
  AuctionManager* getAuctionManager() const;
  CardDeck<Card>* getChanceDeck() const;
  CardDeck<Card>* getCommunityDeck() const;
  CardDeck<Card>* getSkillDeck() const;
  TransactionLogger* getLogger() const;
  GameMaster* getGameMaster() const;    // untuk CardTile & GoToJail

  // Setter
  void setPhase(GamePhase p);
  void setHasExtraTurn(bool val);
  void setHasRolled(bool val);
  void setHasUsedCard(bool val);
  void setGameMaster(GameMaster* gm);  // dipanggil oleh GameMaster konstruktor
  void setCurrTurn(int t);
  void setCurrPlayerIdx(int idx);
  void setMaxTurn(int m);

  // Navigasi giliran
  void advanceTurn();
  void nextPlayer();
  void removePlayer(Player* p);
};
```

---

## Player

```cpp
class Player {
  string id;
  int money;
  Tile* currPetak;
  vector<Property*> listProperti;
  vector<Card*> listCard;             // max 3
  string status;                      // "ACTIVE" | "BANKRUPT" | "JAILED"

  Player(string id, int money, Tile* currPetak,
         vector<Property*> listProperti, vector<Card*> listCard, string status);
  string getID() const;
  int getMoney() const;
  Player& operator=(int amount);      // set uang
  Player& operator-(int amount);      // kurangi uang
  Player& operator+(int amount);      // tambah uang
  int getWealth() const;              // uang + properti + bangunan
  void move(int steps);
  Property* getPropertyAt(int at) const;
  int getPropertyNum() const;
  void showProperty() const;
  Card* getCardAt(int i) const;
  void addCard(Card* card);
  Card* removeCardAt(int i);
  void setStatus(string s);
  string getStatus() const;
};
```

---

## Bank

```cpp
class Bank {
  int housesCount;
  int hotelsCount;

  void payPlayer(Player& p, int amount);         // Bank → Player
  void receiveFromPlayer(Player& p, int amount); // Player → Bank
  void confiscatePlayerAssets(Player& p, GameMaster& gm); // sita & lelang
};
```

---

## Dice

```cpp
class Dice {
  int daduVal1;
  int daduVal2;

  void rollRandom();
  void setManual(int val1, int val2);
  int getVal1() const;
  int getVal2() const;
  int getTotal() const;
  bool isDouble() const;
};
```

---

## Board

```cpp
class Board {
  vector<Tile*> tiles;
  int size;

  Tile* getTile(int index) const;
  Tile* getNextTile(int idx, int step) const;   // wrap-around
  int getSize() const;
};
```

---

## Tile (abstract)

```cpp
class Tile {
  int id;
  string warna;
  string nama;
  string kode;
  string tipeTile;

  virtual void onLanded(Player& p, GameState& gs) = 0;
  int getID() const;
  string getWarna() const;
  string getNama() const;
  string getKode() const;
  string getTipe() const;
};
```

---

## ActionTile subclasses (ringkas)

```cpp
// GoTile
class GoTile : public ActionTile {
  int salary;
  void onLanded(Player&, GameState&) override;  // +salary ke player
  void onPassed(Player&, GameState&);           // +salary saat lewat
};

// JailTile
class JailTile : public ActionTile {
  int fine;
  vector<Player*> inmates;
  vector<Player*> visitors;
  void onLanded(Player&, GameState&) override;  // dianggap visitor
  bool isInmate(Player& p) const;
  void releaseInmate(Player& p);
  void payFine(Player& p, Bank& bank);
  int getFine() const;
  void useJailCard(Player& p);
  void tryEscape(Player& p, Dice& dice);
  void sendToJail(Player& p);
};

// CardTile
class CardTile : public ActionTile {
  CardDeck* deck;                                // pointer ke deck yang sesuai
  void onLanded(Player&, GameState&) override;   // draw & execute kartu
};

// GoToJailTile, FreeParkingTile, TaxTile, FestivalTile
// → masing-masing override onLanded() sesuai mekanisme
```

---

## PropertyTile (abstract)

```cpp
class PropertyTile : public Tile {
  Property* property;

  Property* getProperty() const;
  virtual double calculateRent(int diceTotal) = 0;
  void onLanded(Player&, GameState&) override;
};
// StreetTile, RailroadTile, UtilityTile → override calculateRent()
```

---

## Property (abstract)

```cpp
class Property {
  int id;
  string code, name, colorGroup;
  double purchasePrice, mortgageValue;
  PropertyStatus status;              // BANK | OWNED | MORTGAGED
  string ownerId;

  int getId() const;
  string getCode() const;
  string getName() const;
  string getColorGroup() const;
  double getPurchasePrice() const;
  double getMortgageValue() const;
  PropertyStatus getStatus() const;
  string getOwner() const;
  void setStatus(PropertyStatus s);
  void setOwner(string owner);
  void clearOwner();
  virtual double calculateRentalPrice() = 0;
  virtual double calculateSellPrice() = 0;
  string formattingTXT() const;
  friend ostream& operator<<(ostream& os, const Property& p);
};
```

---

## StreetProperty

```cpp
class StreetProperty : public Property {
  double houseUpgCost, hotelUpgCost;
  map<int, double> rentPrice;         // level (0-5) → rent
  int buildingCount;
  bool hasHotel;
  int maxHouse;                       // = 4
  int festivalMultiplier;             // 1 | 2 | 4 | 8
  int festivalDuration;               // 0–3

  StreetProperty(int, string, string, string, double, double,
                 double, double, map<int,double>);
  double getHouseUpgCost() const;
  double getHotelUpgCost() const;
  int getBuildingCount() const;
  bool getHasHotel() const;
  int getFestivalMultiplier() const;
  int getFestivalDuration() const;
  void setFestivalMultiplier(int m);
  void setFestivalDuration(int d);
  void buildHouse();
  void buildHotel();
  double sellAllBuildings();
  void resetFestival();
  double calculateRentalPrice(bool isMonopoly) const;
  double calculateSellPrice() const override;
};
```

---

## RailroadProperty

```cpp
class RailroadProperty : public Property {
  map<int, double> rentPrice;         // jumlah railroad → rent

  void setOwner(string owner);
  double calculateRentalPrice() const; // butuh jumlah railroad pemilik
  double calculateSellPrice() const override;
};
```

---

## UtilityProperty

```cpp
class UtilityProperty : public Property {
  map<int, double> rentFactor;        // jumlah utility → multiplier

  void setOwner(string owner);
  double calculateRentalPrice() const; // butuh diceTotal & jumlah utility pemilik
  double calculateSellPrice() const override;
};
```

---

## Card (abstract)

```cpp
class Card {
  string type;
  string description;

  virtual ~Card();
  virtual void execute(Player& p, GameMaster& gm) = 0;
  virtual string getDescription() const = 0;
  string getType() const;
};
```

---

## SkillCard (abstract)

```cpp
class SkillCard : public Card {
  bool used;

  void execute(Player& p, GameMaster& gm) override = 0;
  bool isUsed() const;
  void markUsed();
};

// Concrete SkillCards:
class MoveCard : public SkillCard { int steps; };
class DiscountCard : public SkillCard {
  double discountPercent;
  int duration;
  int getDuration() const;
  void decreaseDuration();
};
class ShieldCard : public SkillCard {
  int duration;
  int getDuration() const;
  void decreaseDuration();
};
class TeleportCard : public SkillCard {};
class LassoCard : public SkillCard {};
class DemolitionCard : public SkillCard {};
```

---

## ChanceCard (abstract) & subclasses

```cpp
class ChanceCard : public Card { /* abstract */ };
class NearestStationCard : public ChanceCard {
  void execute(Player& p, GameMaster& gm) override; // ke stasiun terdekat
};
class MoveBackThreeCard : public ChanceCard {
  void execute(Player& p, GameMaster& gm) override; // mundur 3 petak
};
class GoToJailCard : public ChanceCard {
  void execute(Player& p, GameMaster& gm) override; // ke penjara
};
```

---

## GeneralFundCard (abstract) & subclasses

```cpp
class GeneralFundCard : public Card { /* abstract */ };
class BirthdayCard : public GeneralFundCard {
  double amountPerPlayer;           // default 100
  BirthdayCard(double amountPerPlayer = 100);
  void execute(Player& p, GameMaster& gm) override; // terima dari semua pemain
};
class DoctorFeeCard : public GeneralFundCard {
  double fee;                       // default 700
  DoctorFeeCard(double fee = 700);
  void execute(Player& p, GameMaster& gm) override; // bayar ke Bank
};
class ElectionCard : public GeneralFundCard {
  double amountPerPlayer;           // default 200
  ElectionCard(double amountPerPlayer = 200);
  void execute(Player& p, GameMaster& gm) override; // bayar ke semua pemain
};
```

---

## CardDeck<T> (Generic)

```cpp
template<typename T>
class CardDeck {
  vector<unique_ptr<T>> drawPile;
  vector<unique_ptr<T>> discardPile;

  CardDeck();
  bool isEmpty() const;
  const vector<unique_ptr<T>>& getDrawPile() const;
  const vector<unique_ptr<T>>& getDiscardPile() const;
  void pushToDrawPile(unique_ptr<T> card);
  T* draw();                        // reshuffle dari discard jika habis
  void discard(unique_ptr<T> card);
  void shuffle();
  void reshuffleDiscard();
  int drawPileSize() const;
  int discardPileSize() const;
};
```

---

## AuctionManager

```cpp
class AuctionManager {
  PropertyTile* currentProperty;
  int currentHighestBid;
  vector<Player*> activeBidders;
  int passCount;
  Player* highestBidder;

  void placeBid(Player& p, int amount);   // validasi & update bid
  void passBid(Player& p);               // kurangi partisipan aktif
  void resolveAuction();                 // distribusi properti ke pemenang
  bool isAuctionOver() const;
};
```

---

## TransactionLogger & LogEntry

```cpp
struct LogEntry {
  int turn;
  string username;
  string actionType;
  string detail;
  string toString() const;   // "[Turn X] user | ACTION | detail"
};

class TransactionLogger {
  vector<LogEntry> entries;

  TransactionLogger();
  void log(int turn, string username, string actionType, string detail);
  const vector<LogEntry>& getAll() const;
  vector<LogEntry> getLast(int n) const;
  void clear();
  string serialize() const;
  void deserialize(string data);
  int size() const;
};
```

---

## DisplayManager

```cpp
class DisplayManager {
  BoardRenderer& boardRenderer;
  PropertyRenderer& propRenderer;

  DisplayManager(BoardRenderer& br, PropertyRenderer& pr);
  void printBoard(GameMaster& gm);
  void printPropertyDeed(Property& prop);
  void printPlayerProperties(Player& p);
  void printTransactionLog(TransactionLogger& logger, int n = -1); // -1 = all
  void printWinner(GameMaster& gm);
  void printMessage(string msg);
};
```

---

## InputHandler

```cpp
class InputHandler {
  // No attributes — pakai cin langsung
  string readCommand();                         // uppercase
  int readInt(string prompt, int min, int max); // validasi range
  string readString(string prompt);
  bool readConfirm(string prompt);              // y/n → bool
};
```

---

## ConfigLoader

```cpp
class ConfigLoader {
  string basePath;

  vector<PropertyData> loadProperties();
  map<int,int> loadRailroad();
  map<int,int> loadUtility();
  TaxConfig loadTax();
  SpecialConfig loadSpecial();
  MiscConfig loadMisc();
  vector<string> parseLine(string line);
  ifstream openFile(string filename);   // lempar ConfigException jika gagal
};
```

---

## SaveLoadManager

```cpp
class SaveLoadManager {
  void save(GameMaster& gm, string filename);
  GameState load(string filename);
  bool fileExists(string filename) const;
private:
  string serializeState(GameMaster& gm);
  string serializeProperties(Board& board);
  string serializeDeck(Deck& deck);
  GameState deserializeState(string data);
  bool validateFormat(string data);
};
```

---

## Command (abstract) + contoh subclass

```cpp
class Command {
  virtual void execute(GameMaster& gm) = 0;   // GameMaster di-pass sebagai parameter
};

// ✅ IMPLEMENTED
class LemparDaduCommand : public Command {
  GameMaster& gameMaster;   // referensi, bukan pointer
  Player* currentPlayer;
  Dice& dice;
  LemparDaduCommand(GameMaster& gm, Player* player, Dice& d);
  void execute(GameMaster& gm) override;
  // Logic: guard hasRolled → rollRandom → cek 3x double → sendPlayerToJail()
  //        → movePlayer(total) → isDouble → setExtraTurn(true) + reset hasRolled
};

// ✅ IMPLEMENTED
class BeliCommand : public Command {
  Player*   buyer;
  Property* property;    // bukan PropertyTile — langsung Property*
  Bank*     bank;
  bool      playerChoseToBuy;
  BeliCommand(Player* buyer, Property* property, Bank* bank, bool buy);
  void execute(GameMaster& gm) override;
  // buy=true : canAfford? → receiveFromPlayer + setOwner + addProperty
  //            tidak mampu → startAuction()
  // buy=false: skip → startAuction()
};

class AturDaduCommand : public Command {
  GameMaster& gameMaster;
  Player* activePlayer;
  Dice& dice;
  int val1, val2;
  void execute(GameMaster& gm) override;  // validasi 1-6, setManual, gerak
  // throws InvalidDiceValueException
};

class BayarSewaCommand : public Command {
  Player* tenant;
  Player* owner;
  PropertyTile* prop;
  void execute(GameMaster& gm) override;  // hitung sewa, transfer, atau bankruptcy
};

class BayarPajakCommand : public Command {
  Player* p;
  TaxTile* taxTile;
  Bank* bank;
  void execute(GameMaster& gm) override;  // PPH (pilih flat/%) atau PBM (flat langsung)
};

class GadaiCommand : public Command {
  Player* p;
  PropertyTile* prop;
  Bank* bank;
  void execute(GameMaster& gm) override;  // validasi, ubah status, bank->payPlayer
  // throws PropertyHasBuildingsException
};

class TebusCommand : public Command {
  Player* p;
  PropertyTile* prop;
  Bank* bank;
  void execute(GameMaster& gm) override;  // cek MORTGAGED, bayar harga beli, OWNED
};

class LelangCommand : public Command {
  AuctionManager* am;
  PropertyTile* prop;
  vector<Player*> participants;
  void execute(GameMaster& gm) override;  // rotasi bid/pass, resolveAuction
};
```

---

## Exception Hierarchy (ringkas)

```cpp
// Base
class NimonsPoliException : public std::exception {
  string message;
  virtual const char* what() const noexcept override;
};

// Semua child cukup constructor(string message)
class InsufficientFundsException      : public NimonsPoliException {};
class InvalidBidException             : public NimonsPoliException {};
class PropertyHasBuildingsException   : public NimonsPoliException {};
class InvalidPropertyStatusException  : public NimonsPoliException {};
class InvalidDiceValueException       : public NimonsPoliException {};
class InvalidGameMasterCreationException : public NimonsPoliException {};
class InvalidGameMasterStatusException   : public NimonsPoliException {};
class InvalidGameMasterNextTurnException : public NimonsPoliException {};
class InvalidPlayerCreationException  : public NimonsPoliException {};
class InvalidPlayerMoneyException     : public NimonsPoliException {};
class InvalidPlayerPropertyException  : public NimonsPoliException {};
class InvalidPlayerCardUsedException  : public NimonsPoliException {};
class InvalidPlayerMoveException      : public NimonsPoliException {};
class InvalidPlayerStatusException    : public NimonsPoliException {};
```

---

## GUI Layer (Implemented)

### GUIManager

```cpp
class GUIManager {
  Window window;
  IScreen* currentScreen;
  GameMaster* gameMaster;        // di-set via setGameMaster()
  queue<Command*> commandQueue;

  GUIManager(int w, int h, string& title, int fps);
  void setScreen(IScreen* screen);
  void setGameMaster(GameMaster* gm);
  GameMaster* getGameMaster() const;
  void pushCommand(Command* cmd);   // antri command, dieksekusi di flushCommands()
  void run();                       // main loop: flushCommands → update → render
  void shutdown();
  IScreen* getCurrentScreen() const;
  Window& getWindow();
private:
  void flushCommands();   // eksekusi semua command di queue via cmd->execute(*gameMaster)
};
```

### GameScreen (Mock State)

```cpp
class GameScreen : public IScreen {
  // Mock structs: MockProperty, MockPlayer, MockGameState
  // Akan diganti dengan data real dari GameMaster saat integrasi

  void setGUIManager(GUIManager* gm);  // inject GUIManager untuk pushCommand
  void setPlayerCount(int n);

  // State dadu
  struct DiceState {
    int val1, val2;
    bool hasRolled, isDouble, tripleDouble, animating;
    float animTimer;
    static constexpr float ANIM_DURATION = 0.6f;
  };

  // State dialog beli
  struct BuyDialogState {
    bool visible;
    int  tileIdx;    // indeks petak yang diinjak
    bool canAfford;
  };

  // Key methods
  void handleLemparDadu();        // mock: simulasi roll + triggerBuyDialog jika STREET+BANK
                                  // real: push LemparDaduCommand ke GUIManager
  void triggerBuyDialog(int idx); // tampilkan dialog beli/skip
  void drawBuyDialog();           // render dialog + handle klik BELI/SKIP
                                  // real: push BeliCommand ke GUIManager
  void drawDiceArea();            // render dua dadu animasi di tengah board
};
```

### Window

```cpp
class Window {
  // Konstruktor memanggil InitWindow() + SetTargetFPS()
  // Destruktor: if (IsWindowReady()) CloseWindow()  ← fix segfault double-close
  Window(int w, int h, string& title, int fps);
  bool isOpen() const;    // return !WindowShouldClose()
  void beginFrame();
  void endFrame();
};
```
