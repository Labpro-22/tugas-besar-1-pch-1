# PROGRESS — Nimonspoli (Kelompok PCH)

> Update file ini di akhir setiap sesi.

**Deadline M2:** 24 April 2026 pukul 23.59

> **Update sesi terakhir:** Integrasi GameMaster ↔ GUI selesai. GameScreen kini berjalan di mode real (bukan mock). BoardFactory, PropertyFactory, ConfigLoader, SaveLoadManager sudah terhubung di `main.cpp`. Player bisa bergerak mengelilingi board, dialog beli muncul, END TURN berfungsi. Beberapa bug aktif ditemukan dan sebagian sudah diperbaiki.

---

## STATUS KOMPONEN

> Legend: ✅ Selesai | 🔄 Ada tapi perlu fix/lanjutan | ⬜ Belum ada | ❌ Bug aktif

catatan: komponen dengan lambang 🔄 memiliki kemungkinan untuk sudah diimplementasi selama chat berlangsung oleh anggota tim lain.

### DATA ACCESS LAYER

| Komponen          | Status | Catatan                                                                                                                                           |
| ----------------- | ------ | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ConfigLoader`    | ✅     | Baca `property.txt`, `railroad.txt`, `utility.txt`, `tax.txt`, `special.txt`, `misc.txt`. Path config: `config/` di root repo                     |
| `SaveLoadManager` | ✅     | Sudah dipakai di `main.cpp` untuk Load Game path                                                                                                  |
| `BoardFactory`    | ✅     | Membuat 40 tile dari `PropertyData` + `ActionData`. Bug deck tertukar (DNU↔KSP) sudah diperbaiki. Bug urutan tile via `std::map` sudah diperbaiki |
| `PropertyFactory` | ✅     | Static method `createProperties()` membuat semua `Property*` dari config                                                                          |

### GAME LOGIC LAYER

| Komponen         | Status | Catatan                                                                                                                                              |
| ---------------- | ------ | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| `GameMaster`     | ✅     | Konstruktor memanggil `state.setGameMaster(this)`. `beginTurn()`, `endTurn()`, `movePlayer()`, `sendPlayerToJail()`, `releaseFromJail()` implemented |
| `GameState`      | ✅     | Menyimpan `GameMaster*`, semua getter/setter lengkap                                                                                                 |
| `Board`          | ✅     | `.hpp` + `.cpp` lengkap, `setProperty()` ditambahkan ke `PropertyTile`                                                                               |
| `Bank`           | ✅     | `.hpp` + `.cpp` lengkap & sudah compile                                                                                                              |
| `Dice`           | ✅     | `.hpp` + `.cpp` lengkap & sudah compile                                                                                                              |
| `Player`         | ✅     | `.hpp` + `.cpp` lengkap & sudah compile                                                                                                              |
| `AuctionManager` | ✅     | `.hpp` + `.cpp` lengkap & sudah compile                                                                                                              |
| `CardDeck<T>`    | ✅     | Fixed: pakai `T*`, implementasi inline di `.hpp`                                                                                                     |

### TILE HIERARCHY

| Komponen                  | Status | Catatan                                                                   |
| ------------------------- | ------ | ------------------------------------------------------------------------- |
| `Tile` (abstract)         | ✅     | Di `Board.cpp`                                                            |
| `ActionTile` (abstract)   | ✅     | Di `Board.cpp`                                                            |
| `GoTile`                  | ✅     | Di `Board.cpp`                                                            |
| `JailTile`                | ✅     | `onLanded()` set visitor (bukan inmate). `sendToJail()` set status JAILED |
| `GoToJail`                | ✅     | `onLanded()` pakai `gm->sendPlayerToJail()` via `gs.getGameMaster()`      |
| `FreeParkingTile`         | ✅     | Di `Board.cpp` (no-op)                                                    |
| `TaxTile`                 | 🔄     | Stub — hanya set phase, `BayarPajakCommand` belum dihubungkan             |
| `CardTile`                | ✅     | `onLanded()` memanggil `kartu->execute(p, *gm)` via `gs.getGameMaster()`  |
| `FestivalTile`            | 🔄     | Stub — `FestivalCommand` belum ada                                        |
| `PropertyTile` (abstract) | ✅     | Di `Board.cpp`. Tambah `setProperty()` untuk BoardFactory                 |
| `StreetTile`              | 🔄     | `onLanded()` set `AWAITING_BUY`. Buy dialog muncul di GUI                 |
| `RailroadTile`            | 🔄     | `onLanded()` langsung set OWNED ke pemain pertama yang mendarat           |
| `UtilityTile`             | 🔄     | `onLanded()` langsung set OWNED ke pemain pertama yang mendarat           |

### PROPERTY HIERARCHY

| Komponen              | Status | Catatan                                                   |
| --------------------- | ------ | --------------------------------------------------------- |
| `Property` (abstract) | 🔄     | `clearOwner()` assign `nullptr` ke `string` → perlu fix   |
| `StreetProperty`      | 🔄     | Header + `.cpp` ada, `formattingTXT()` perlu dikonfirmasi |
| `RailroadProperty`    | 🔄     | Stub `.cpp` dibuat, `formattingTXT()` kosong              |
| `UtilityProperty`     | 🔄     | Stub `.cpp` dibuat, `formattingTXT()` kosong              |

### CARD HIERARCHY

| Komponen                                      | Status | Catatan                                             |
| --------------------------------------------- | ------ | --------------------------------------------------- |
| `Card` (abstract)                             | ✅     | Butuh `class GameMaster;` forward declare di `.hpp` |
| `ChanceCard` + 3 subclass                     | ✅     |                                                     |
| `GeneralFundCard` + 3 subclass                | ✅     |                                                     |
| `SkillCard` (abstract)                        | ✅     |                                                     |
| `MoveCard`, `DiscountCard`, `ShieldCard`      | ✅     |                                                     |
| `TeleportCard`, `LassoCard`, `DemolitionCard` | ✅     |                                                     |

### EXCEPTION HIERARCHY

| Komponen                         | Status | Catatan                                          |
| -------------------------------- | ------ | ------------------------------------------------ |
| `NimonsPoliException` + 14 child | ⬜     | Belum ada — dibutuhkan untuk validasi di Command |

### UI LAYER

| Komponen            | Status | Catatan                                             |
| ------------------- | ------ | --------------------------------------------------- |
| `DisplayManager`    | ⬜     | Tidak diprioritaskan (pakai GUI)                    |
| `BoardRenderer`     | ⬜     | Tidak diprioritaskan (pakai GUI)                    |
| `PropertyRenderer`  | ⬜     | Tidak diprioritaskan (pakai GUI)                    |
| `InputHandler`      | ⬜     | Tidak diprioritaskan (pakai GUI)                    |
| `TransactionLogger` | ✅     | `addLog()`, `getLogs()`, `getLastLogs()`, `clear()` |

### COMMAND PATTERN

| Komponen              | Status | Catatan                                                                                                                        |
| --------------------- | ------ | ------------------------------------------------------------------------------------------------------------------------------ |
| `Command` (abstract)  | ✅     | `execute(GameMaster& gm)` — ada di `Command.hpp`                                                                               |
| `LemparDaduCommand`   | ✅     | Roll, cek 3× double → jail, double → extra turn, movePlayer, reset phase ke PLAYER_TURN setelah landing                        |
| `BeliCommand`         | ✅     | Beli (deduct balance, setOwner) atau skip → startAuction                                                                       |
| Semua Command lainnya | ⬜     | `BayarSewaCommand`, `BayarPajakCommand`, `GadaiCommand`, `TebusCommand`, `LelangCommand`, `AturDaduCommand`, `FestivalCommand` |

### GUI LAYER (Bonus)

| Komponen        | Status | Catatan                                                                                                                                            |
| --------------- | ------ | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| `GUIManager`    | ✅     | `pushCommand()`, `flushCommands()`, `syncDiceResult()` dipanggil dari game loop di `main.cpp`                                                      |
| `GameScreen`    | ✅     | **Mode real aktif** — `syncFromGameMaster()` sync data tiap frame. Render papan, player panel, log popup, dadu animasi, buy dialog semua berfungsi |
| Panel & Dialog  | 🔄     | `BuyDialogState` (beli/skip) berfungsi di mode real. Dialog lelang, gadai, tebus belum ada                                                         |
| `Window`        | ✅     | Destruktor fix: `if (IsWindowReady()) CloseWindow()`                                                                                               |
| END TURN button | ✅     | Memanggil `endTurn()` + `beginTurn()`, giliran berpindah ke pemain berikutnya                                                                      |
| COM auto-play   | 🔄     | Kerangka ada di `main.cpp`, menunggu `ComputerPlayer::executeTurn()`                                                                               |

### OOP REQUIREMENTS

| Requirement                 | Status | Catatan                                                            |
| --------------------------- | ------ | ------------------------------------------------------------------ |
| Inheritance                 | ✅     | Tile, Property, Card, Command hierarchy lengkap                    |
| Polymorphism                | ✅     | `onLanded()`, `calculateRent()`, `execute()` via virtual           |
| Exception Handling          | ⬜     | Belum ada exception class                                          |
| Operator Overloading        | ✅     | `Player::operator+=/-=/>/<`                                        |
| Abstract Class + Virtual    | ✅     | Tile, Property, Card, Command                                      |
| Generic Class `CardDeck<T>` | ✅     | Fixed dengan `T*`                                                  |
| STL: vector + map           | ✅     | Dipakai di Board, Player, Property, BoardFactory                   |
| Layered Architecture        | ✅     | Data Access (Config/Factory/SaveLoad), Game Logic, UI/GUI terpisah |
| Command Pattern             | 🔄     | `LemparDaduCommand` ✅, `BeliCommand` ✅, sisanya ⬜               |

---

## BUG AKTIF

| #   | Komponen                                | Deskripsi                                                                                                                                                                                          | Status                                        |
| --- | --------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------- |
| 1   | `GameScreen::drawPlayers()`             | Posisi pion hanya benar di sisi BOTTOM. Sisi LEFT/RIGHT/TOP pion tidak di tengah tile karena tidak ada offset rotasi                                                                               | 🔄 Patch tersedia, belum dikonfirmasi applied |
| 2   | `LemparDaduCommand::execute()`          | Player berstatus JAILED masih bisa lempar dadu — tidak ada guard cek `PlayerStatus::JAILED`                                                                                                        | 🔄 Patch tersedia, belum dikonfirmasi applied |
| 3   | `GameMaster::movePlayer()`              | Kondisi deteksi melewati GO `targetIdx < curIdx` false-positive → player bisa tiba-tiba pindah posisi aneh. Fix: gunakan `(curIdx + steps) >= boardSize`                                           | 🔄 Patch tersedia, belum dikonfirmasi applied |
| 4   | `JailTile::onLanded()` + `BoardFactory` | Player mendarat di PEN (kunjungan) langsung di-set JAILED, padahal harusnya ACTIVE. Hanya mendarat di PPJ yang boleh set JAILED. Perlu dicek apakah `BoardFactory` mapping PEN dan PPJ sudah benar | 🔄 Investigasi: kirim hasil debug             |

---

## TODO PENTING (belum diimplementasi)

1. **Command-command lain** — `BayarSewaCommand`, `BayarPajakCommand`, `GadaiCommand`, `TebusCommand`, `LelangCommand`, `AturDaduCommand`, `FestivalCommand`

2. **`NimonsPoliException` + 14 child** — dibutuhkan untuk validasi di Command

---
