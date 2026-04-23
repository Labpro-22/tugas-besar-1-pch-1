# ARCHITECTURE — Nimonspoli (C++ OOP)

> Desain kelas kelompok PCH. Update terakhir berdasarkan Laporan M1.
> Peta hubungan antar class. Gunakan file ini sebagai referensi utama saat coding.

---

## LAYERED ARCHITECTURE

```
┌──────────────────────────────────────────────────────────┐
│                     UI LAYER                             │
│  DisplayManager · BoardRenderer · PropertyRenderer       │
│  InputHandler · TransactionLogger · LogEntry             │
│  [Command Pattern]: Command (abstract) + semua subclass  │
├──────────────────────────────────────────────────────────┤
│                  GAME LOGIC LAYER                        │
│  GameMaster · GameState · AuctionManager                 │
│  Bank · Dice · Player                                    │
│  Board · Tile hierarchy · Property hierarchy             │
│  Card hierarchy · CardDeck<T>                            │
├──────────────────────────────────────────────────────────┤
│                DATA ACCESS LAYER                         │
│  ConfigLoader · SaveLoadManager                          │
└──────────────────────────────────────────────────────────┘
```

> Catatan: TransactionLogger ada di UI Layer karena di-inject ke Command.
> Command Pattern dipakai untuk memisahkan UI ↔ Game Logic.

---

## CLASS OVERVIEW

### DATA ACCESS LAYER

| Class             | Tanggung Jawab                                                                                             |
| ----------------- | ---------------------------------------------------------------------------------------------------------- |
| `ConfigLoader`    | Baca semua file config (property.txt, railroad.txt, dll). Satu-satunya kelas yang boleh akses file config. |
| `SaveLoadManager` | Serialisasi/deserialisasi seluruh state game ke/dari file .txt                                             |

### GAME LOGIC LAYER

| Class            | Tanggung Jawab                                                                 |
| ---------------- | ------------------------------------------------------------------------------ |
| `GameMaster`     | Controller utama: main loop, giliran, kondisi menang, handle bankruptcy        |
| `GameState`      | Snapshot data global: currTurn, maxTurn, listPlayer, Board, Bank, Deck         |
| `Board`          | Menyimpan vector<Tile\*>, lookup tile by index                                 |
| `Bank`           | Pengelola sirkulasi uang: payPlayer, receiveFromPlayer, confiscatePlayerAssets |
| `Dice`           | Generate angka dadu (random/manual)                                            |
| `Player`         | Data & aksi pemain: uang, posisi, properti, kartu tangan, status               |
| `AuctionManager` | Kelola antrean lelang, validasi bid, distribusi properti                       |
| `CardDeck<T>`    | Generic deck untuk semua tumpukan kartu                                        |

### UI LAYER

| Class               | Tanggung Jawab                                             |
| ------------------- | ---------------------------------------------------------- |
| `DisplayManager`    | Satu-satunya pintu output ke terminal (wraps renderer)     |
| `BoardRenderer`     | Menghasilkan string representasi papan 11×11 + legenda     |
| `PropertyRenderer`  | Menghasilkan string akta properti & daftar properti pemain |
| `InputHandler`      | Satu-satunya pintu input dari keyboard                     |
| `TransactionLogger` | Catat semua kejadian penting ke vector<LogEntry>           |
| `LogEntry`          | Satu unit kejadian: turn, username, actionType, detail     |

### COMMAND PATTERN (UI Layer)

| Command                | Perintah               |
| ---------------------- | ---------------------- |
| `CetakPapanCommand`    | CETAK_PAPAN            |
| `CetakAktaCommand`     | CETAK_AKTA             |
| `CetakPropertiCommand` | CETAK_PROPERTI         |
| `CetakLogCommand`      | CETAK_LOG [N]          |
| `SimpanCommand`        | SIMPAN                 |
| `MuatCommand`          | MUAT                   |
| `LemparDaduCommand`    | LEMPAR_DADU            |
| `AturDaduCommand`      | ATUR_DADU X Y          |
| `BeliCommand`          | BELI (otomatis)        |
| `BayarSewaCommand`     | BAYAR_SEWA (otomatis)  |
| `BayarPajakCommand`    | BAYAR_PAJAK (otomatis) |
| `GadaiCommand`         | GADAI                  |
| `TebusCommand`         | TEBUS                  |
| `LelangCommand`        | LELANG (otomatis)      |

---

## TILE HIERARCHY

```
Tile (abstract)
├── onLanded(Player&, GameState&) = 0  [virtual]
├── ActionTile (abstract)
│   ├── GoTile             [salary]
│   ├── JailTile           [fine, vector inmates, vector visitors]
│   ├── GoToJailTile
│   ├── FreeParkingTile
│   ├── TaxTile
│   ├── CardTile           [Deck* → CardDeck]
│   └── FestivalTile
└── PropertyTile (abstract)
    ├── Property*          [pointer ke data properti]
    ├── StreetTile
    ├── RailroadTile
    └── UtilityTile
```

---

## PROPERTY HIERARCHY

```
Property (abstract)
├── id, code, name, colorGroup
├── purchasePrice, mortgageValue
├── status (BANK | OWNED | MORTGAGED)
├── ownerId
├── calculateRentalPrice() = 0
├── calculateSellPrice() = 0
├── StreetProperty
│   ├── houseUpgCost, hotelUpgCost
│   ├── rentPrice: map<int, double>    (level → rent)
│   ├── buildingCount, hasHotel, maxHouse
│   ├── festivalMultiplier (1/2/4/8)
│   └── festivalDuration (0–3)
├── RailroadProperty
│   └── rentPrice: map<int, double>    (jumlah railroad → rent)
└── UtilityProperty
    └── rentFactor: map<int, double>   (jumlah utility → multiplier)
```

> Catatan: PropertyTile (Tile) dan Property (entity) adalah dua kelas berbeda.
> PropertyTile menyimpan pointer ke Property.

---

## CARD HIERARCHY

```
Card (abstract)
├── type: string
├── description: string
├── execute(Player&, GameMaster&) = 0
├── getDescription() = 0
├── ChanceCard (abstract)
│   ├── NearestStationCard
│   ├── MoveBackThreeCard
│   └── GoToJailCard
├── GeneralFundCard (abstract)
│   ├── BirthdayCard       [amountPerPlayer = 100]
│   ├── DoctorFeeCard      [fee = 700]
│   └── ElectionCard       [amountPerPlayer = 200]
└── SkillCard (abstract)
    ├── used: bool
    ├── MoveCard            [steps: int]
    ├── DiscountCard        [discountPercent, duration]
    ├── ShieldCard          [duration]
    ├── TeleportCard
    ├── LassoCard
    └── DemolitionCard

CardDeck<T>                 [Generic — drawPile + discardPile, unique_ptr<T>]
```

---

## EXCEPTION HIERARCHY

```
std::exception
└── NimonsPoliException (abstract)
    ├── message: string
    ├── what() const noexcept
    ├── InsufficientFundsException
    ├── InvalidBidException
    ├── PropertyHasBuildingsException
    ├── InvalidPropertyStatusException
    ├── InvalidDiceValueException
    ├── InvalidGameMasterCreationException
    ├── InvalidGameMasterStatusException
    ├── InvalidGameMasterNextTurnException
    ├── InvalidPlayerCreationException
    ├── InvalidPlayerMoneyException
    ├── InvalidPlayerPropertyException
    ├── InvalidPlayerCardUsedException
    ├── InvalidPlayerMoveException
    └── InvalidPlayerStatusException
```

---

## BONUS: GUI LAYER (jika dikerjakan)

```
GUIManager
├── currentScreen: IScreen*
├── commandQueue: queue<Command*>
├── Window (composition)
├── IScreen (abstract)
│   ├── MainMenuScreen     [Button btnNewGame, btnLoadGame]
│   ├── GameScreen         [BoardPanel, PlayerInfoPanel, ActionPanel, LogPanel]
│   └── WinScreen          [Button btnMainMenu, btnExit]
├── Panel
│   ├── BoardPanel         [BoardRenderer&, tileSize, selectedTile]
│   ├── ActionPanel        [vector<Button>, GUIManager&]
│   ├── PlayerInfoPanel    [map avatarColors]
│   └── LogPanel           [TransactionLogger&, maxVisible]
└── Dialog (iDialog abstract)
    ├── BuyDialog
    ├── AuctionDialog
    ├── LiquidationDialog
    ├── PlayerSetupDialog
    └── FilePickerDialog
```

---

## DEPENDENCY MAP (ringkas)

```
GameMaster ──→ GameState ──→ Board, vector<Player*>, Bank, Deck
GameMaster ──→ AuctionManager
GameMaster ──→ TransactionLogger (logger)
GameState  ──→ GameMaster* (back-reference, di-set oleh konstruktor GameMaster)
               Digunakan oleh CardTile::onLanded() dan GoToJail::onLanded()

Command (abstract) ──→ GameMaster (via execute(GameMaster& gm))
  LemparDaduCommand ──→ GameMaster&, Dice&, Player*
  BeliCommand        ──→ Player*, Property*, Bank*, bool playerChoseToBuy
  BayarSewaCommand   ──→ Player* tenant, Player* owner, PropertyTile*
  BayarPajakCommand  ──→ Player*, TaxTile*, Bank*
  GadaiCommand       ──→ Player*, PropertyTile*, Bank*
  TebusCommand       ──→ Player*, PropertyTile*, Bank*
  LelangCommand      ──→ AuctionManager*, PropertyTile*, vector<Player*>

DisplayManager ──aggregation──→ BoardRenderer, PropertyRenderer
DisplayManager ──dependency──→ TransactionLogger, GameMaster

GUIManager ──→ Window, IScreen*, GameMaster*, queue<Command*>
GameScreen ──→ GUIManager* (inject via setGUIManager)
GameScreen ──pushCommand──→ GUIManager ──execute──→ GameMaster

PropertyTile ──→ Property* (data properti)
CardTile     ──→ CardDeck* + gs.getGameMaster() untuk execute kartu
JailTile     ──→ vector<Player*> inmates, visitors
GoToJail     ──→ gs.getGameMaster()->sendPlayerToJail()
```

---

## OPERATOR OVERLOADING (requirement)

```cpp
// Player.h
Player& operator=(int amount);    // set uang
Player& operator-(int amount);    // kurangi uang
Player& operator+(int amount);    // tambah uang
bool operator>(const Player& other) const;  // bandingkan kekayaan
bool operator<(const Player& other) const;
```

---

## STL USAGE (requirement)

| Kontainer               | Digunakan Di                        | Kegunaan                 |
| ----------------------- | ----------------------------------- | ------------------------ |
| `vector<Tile*>`         | Board                               | 40 petak papan           |
| `vector<Player*>`       | GameState                           | Daftar semua pemain      |
| `vector<Property*>`     | Player                              | Properti milik pemain    |
| `vector<Card*>`         | Player                              | Kartu tangan (max 3)     |
| `vector<LogEntry>`      | TransactionLogger                   | Riwayat log              |
| `vector<Player*>`       | AuctionManager (activeBidders)      | Peserta lelang aktif     |
| `vector<unique_ptr<T>>` | CardDeck<T> (drawPile, discardPile) | Deck kartu               |
| `map<int,int>`          | RailroadProperty, UtilityProperty   | Tabel sewa               |
| `map<int,double>`       | StreetProperty (rentPrice)          | Tabel sewa per level     |
| `map<string,string>`    | BoardRenderer (colorMap)            | Mapping warna ANSI       |
| `map<string,color>`     | PlayerInfoPanel (avatarColors)      | Warna bidak GUI          |
| `queue<Command*>`       | GUIManager                          | Antrian command klik GUI |

---

## FLOW GILIRAN (ringkas)

```
awal giliran:
  1. Semua pemain dapat 1 SkillCard acak (trigger drop jika > 3)
  2. Cek status JAILED → pilih opsi keluar penjara

aksi pemain (via Command):
  3. [Opsional] GUNAKAN_KEMAMPUAN (SkillCard, sebelum dadu)
  4. LEMPAR_DADU / ATUR_DADU
     → Dice::rollRandom() atau Dice::setManual()
     → Cek double → giliran tambahan (max 3×, ke-3 langsung jail)
     → Gerak bidak → tile->onLanded(player, gameState)
     → Trigger Command otomatis (Beli/Sewa/Pajak/dll)
  5. Perintah manual kapan saja: CETAK_*, GADAI, TEBUS, BANGUN
  6. SIMPAN hanya di awal giliran (sebelum langkah 3)

akhir giliran:
  7. Kurangi festivalDuration properti milik pemain ini
  8. GameMaster::isCompleted() → cek kondisi menang
  9. GameMaster::nextTurn() → pindah ke pemain berikutnya
```

---

## KEPUTUSAN TEKNIS

- **Command Pattern**: setiap aksi pemain = satu kelas Command → UI ↔ Logic decoupled. Signature `execute(GameMaster& gm)` — GameMaster di-pass sebagai parameter
- **Property terpisah dari PropertyTile**: PropertyTile adalah petak di Board, Property adalah data (harga, status, pemilik). PropertyTile menyimpan `Property*`
- **BeliCommand menerima `Property*` bukan `PropertyTile*`**: lebih fleksibel, tidak bergantung pada tile layer
- **CardDeck<T>**: dipakai untuk deck Kesempatan (`CardDeck<ChanceCard>`), Dana Umum (`CardDeck<GeneralFundCard>`), dan Kemampuan Spesial (`CardDeck<SkillCard>`)
- **Aggregation di UI Layer**: semua dependency UI pakai referensi `&` (di-inject via constructor), bukan membuat objek baru → hindari duplikasi, mudah diganti
- **TransactionLogger di-inject** ke semua Command yang perlu mencatat aksi
- **NimonsPoliException** sebagai base exception → `what()` di base, constructor di child → DRY
- **Rule of Zero**: minimalisasi manual constructor/destructor/copy/assign
- **GameState sebagai snapshot**: memisahkan data dari controller (GameMaster)
- **GameState menyimpan `GameMaster*`**: back-reference yang di-set oleh konstruktor GameMaster. Dibutuhkan oleh `CardTile::onLanded()` dan `GoToJail::onLanded()` agar bisa memanggil `GameMaster` tanpa circular dependency di header
- **GUIManager sebagai Command dispatcher**: `pushCommand(cmd*)` antri command, `flushCommands()` eksekusi di awal setiap frame via `cmd->execute(*gameMaster)` lalu `delete cmd`
- **GameScreen dual-mode**: bisa jalan dengan `MockGameState` (tanpa GameMaster) atau mode real (dengan GameMaster via GUIManager). Transisi bertahap tanpa breaking change
- **Window destruktor guard**: `if (IsWindowReady()) CloseWindow()` — mencegah double-close segfault saat Raylib window ditutup user
