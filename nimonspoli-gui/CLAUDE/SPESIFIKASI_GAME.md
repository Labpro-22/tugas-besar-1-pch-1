# RINGKASAN SPESIFIKASI — Nimonspoli

> IF2010 Pemrograman Berorientasi Objek | Tugas Besar 1
> Bahasa: C++ | Compile: Makefile | Target OS: Linux

---

## 1. GAMBARAN UMUM

Game Monopoly berbasis CLI dalam C++. Papan 40 petak (persegi), 2–4 pemain, giliran searah jarum jam.

**Alur Program:**

1. Baca file konfigurasi
2. Menu: `New Game` (input pemain, acak urutan) atau `Load Game`
3. Loop giliran → pemain pilih perintah → cek kondisi selesai

---

## 2. FILE KONFIGURASI

| File           | Isi                                                                                    |
| -------------- | -------------------------------------------------------------------------------------- |
| `property.txt` | ID, KODE, NAMA, JENIS, WARNA, HARGA_LAHAN, NILAI_GADAI, UPG_RUMAH, UPG_HT, RENT_L0..L5 |
| `railroad.txt` | JUMLAH_RAILROAD → BIAYA_SEWA                                                           |
| `utility.txt`  | JUMLAH_UTILITY → FAKTOR_PENGALI                                                        |
| `tax.txt`      | PPH_FLAT, PPH_PERSENTASE, PBM_FLAT                                                     |
| `special.txt`  | GO_SALARY, JAIL_FINE                                                                   |
| `misc.txt`     | MAX_TURN, SALDO_AWAL                                                                   |

---

## 3. PAPAN PERMAINAN (40 Petak)

### Urutan Petak

| Idx | Kode | Nama               | Tipe     | Warna     |
| --- | ---- | ------------------ | -------- | --------- |
| 1   | GO   | Petak Mulai        | Spesial  | -         |
| 2   | GRT  | Garut              | Lahan    | Coklat    |
| 3   | DNU  | Dana Umum          | Kartu    | -         |
| 4   | TSK  | Tasikmalaya        | Lahan    | Coklat    |
| 5   | PPH  | Pajak Penghasilan  | Pajak    | -         |
| 6   | GBR  | Stasiun Gambir     | Stasiun  | -         |
| 7   | BGR  | Bogor              | Lahan    | Biru Muda |
| 8   | FES  | Festival           | Festival | -         |
| 9   | DPK  | Depok              | Lahan    | Biru Muda |
| 10  | BKS  | Bekasi             | Lahan    | Biru Muda |
| 11  | PEN  | Penjara            | Spesial  | -         |
| 12  | MGL  | Magelang           | Lahan    | Pink      |
| 13  | PLN  | PLN                | Utilitas | Abu-abu   |
| 14  | SOL  | Solo               | Lahan    | Pink      |
| 15  | YOG  | Yogyakarta         | Lahan    | Pink      |
| 16  | STB  | Stasiun Bandung    | Stasiun  | -         |
| 17  | MAL  | Malang             | Lahan    | Orange    |
| 18  | DNU  | Dana Umum          | Kartu    | -         |
| 19  | SMG  | Semarang           | Lahan    | Orange    |
| 20  | SBY  | Surabaya           | Lahan    | Orange    |
| 21  | BBP  | Bebas Parkir       | Spesial  | -         |
| 22  | MKS  | Makassar           | Lahan    | Merah     |
| 23  | KSP  | Kesempatan         | Kartu    | -         |
| 24  | BLP  | Balikpapan         | Lahan    | Merah     |
| 25  | MND  | Manado             | Lahan    | Merah     |
| 26  | TUG  | Stasiun Tugu       | Stasiun  | -         |
| 27  | PLB  | Palembang          | Lahan    | Kuning    |
| 28  | PKB  | Pekanbaru          | Lahan    | Kuning    |
| 29  | PAM  | PAM                | Utilitas | Abu-abu   |
| 30  | MED  | Medan              | Lahan    | Kuning    |
| 31  | PPJ  | Pergi ke Penjara   | Spesial  | -         |
| 32  | BDG  | Bandung            | Lahan    | Hijau     |
| 33  | DEN  | Denpasar           | Lahan    | Hijau     |
| 34  | FES  | Festival           | Festival | -         |
| 35  | MTR  | Mataram            | Lahan    | Hijau     |
| 36  | GUB  | Stasiun Gubeng     | Stasiun  | -         |
| 37  | KSP  | Kesempatan         | Kartu    | -         |
| 38  | JKT  | Jakarta            | Lahan    | Biru Tua  |
| 39  | PBM  | Pajak Barang Mewah | Pajak    | -         |
| 40  | IKN  | Ibu Kota Nusantara | Lahan    | Biru Tua  |

### Color Groups

| Warna         | Petak         |
| ------------- | ------------- |
| Coklat (2)    | GRT, TSK      |
| Biru Muda (3) | BGR, DPK, BKS |
| Pink (3)      | MGL, SOL, YOG |
| Orange (3)    | MAL, SMG, SBY |
| Merah (3)     | MKS, BLP, MND |
| Kuning (3)    | PLB, PKB, MED |
| Hijau (3)     | BDG, DEN, MTR |
| Biru Tua (2)  | JKT, IKN      |

---

## 4. MEKANISME UTAMA

### Dadu

- 2d6, random atau manual (`ATUR_DADU X Y`)
- Double → giliran tambahan
- Double 3× berturut → langsung ke Penjara

### Kepemilikan Properti

- **Street**: Tawari beli → jika tidak mau/tidak mampu → lelang otomatis
- **Railroad & Utility**: Otomatis milik pemain pertama yang mendarat, GRATIS, tanpa lelang

### Sewa

- Street: sesuai tabel level (L0–L5). Jika monopoli & 0 bangunan → 2× sewa dasar
- Railroad: tabel berdasarkan jumlah railroad dimiliki pemilik
- Utility: total dadu × faktor pengali (tergantung jumlah utility dimiliki)
- Properti MORTGAGED → tidak ada sewa

### Gadai

- Syarat: status OWNED, tidak ada bangunan di color group
- Jika ada bangunan di color group → jual semua bangunan dulu (½ harga beli)
- Nilai gadai → lihat akta. Tebus = bayar harga beli penuh
- MORTGAGED tidak bisa langsung dijual ke Bank (harus tebus dulu)

### Bangunan (Street Only)

- Syarat: monopoli color group
- Maks 4 rumah per petak, lalu upgrade ke hotel
- Pembangunan harus merata (selisih max 1 antar petak dalam color group)
- Hotel = tingkat tertinggi, tidak bisa dibangun lagi

### Lelang

- Dipicu: pemain tidak beli street / bangkrut ke Bank
- Urutan mulai dari pemain setelah pemicu
- Aksi: PASS atau BID <jumlah>
- Selesai jika (jumlah_pemain - 1) PASS berturut-turut
- Minimal 1 pemain harus BID

### Pajak

- PPH: pilih flat ATAU persentase kekayaan (putuskan SEBELUM hitung kekayaan)
  - Kekayaan = uang tunai + harga beli semua properti + harga beli bangunan
- PBM: langsung bayar flat, tanpa pilihan

### Festival

- Pilih 1 properti milik sendiri → sewa ×2 selama 3 giliran
- Bisa diperkuat hingga 3× (max ×8) dengan memilih properti sama lagi
- Durasi di-reset setiap penguatan

### Kartu Kesempatan

- "Pergi ke stasiun terdekat"
- "Mundur 3 petak"
- "Masuk Penjara"

### Kartu Dana Umum

- "Hari ulang tahun → terima M100 dari tiap pemain"
- "Biaya dokter → bayar M700"
- "Mau nyaleg → bayar M200 ke tiap pemain"

### Kartu Kemampuan Spesial (max 3 di tangan)

| Kartu          | Jumlah | Efek                                   |
| -------------- | ------ | -------------------------------------- |
| MoveCard       | 4      | Maju N petak (N acak saat dapat)       |
| DiscountCard   | 3      | Diskon % acak selama 1 giliran         |
| ShieldCard     | 2      | Kebal tagihan/sanksi 1 giliran         |
| TeleportCard   | 2      | Pindah ke petak manapun                |
| LassoCard      | 2      | Tarik lawan di depan ke posisi sendiri |
| DemolitionCard | 2      | Hancurkan 1 bangunan milik lawan       |

- Dapat 1 kartu acak di awal giliran
- Max 3 kartu → dapat ke-4 → wajib buang 1
- Hanya bisa digunakan SEBELUM lempar dadu, max 1× per giliran

### Penjara

- Masuk jika: mendarat di PPJ, kartu penjara, atau double 3×
- Keluar: bayar denda, gunakan kartu bebas penjara, atau lempar double (max 3 giliran)
- Giliran ke-4 wajib bayar denda

### Bangkrut

- Tidak mampu bayar → hitung potensi likuidasi
- Jika likuidasi cukup → wajib likuidasi
- Jika tidak cukup → bangkrut
- Bangkrut ke pemain: semua aset ke kreditor
- Bangkrut ke Bank: uang ke Bank, properti dilelang

### Kondisi Menang

- **MAX_TURN**: pemain uang terbanyak menang (tiebreak: properti terbanyak → kartu terbanyak → semua menang)
- **BANKRUPTCY** (MAX_TURN < 1): main terus sampai 1 pemain tersisa

---

## 5. SEMUA PERINTAH

| Perintah              | Trigger  | Keterangan                                        |
| --------------------- | -------- | ------------------------------------------------- |
| CETAK_PAPAN           | Manual   | Tampilkan papan dengan warna, bidak, kepemilikan  |
| LEMPAR_DADU           | Manual   | Lempar dadu random                                |
| ATUR_DADU X Y         | Manual   | Set dadu manual                                   |
| CETAK_AKTA            | Manual   | Info lengkap properti                             |
| CETAK_PROPERTI        | Manual   | Semua properti milik pemain aktif                 |
| BELI                  | Otomatis | Dipicu saat mendarat di properti BANK             |
| BAYAR_SEWA            | Otomatis | Dipicu saat mendarat di properti OWNED orang lain |
| BAYAR_PAJAK           | Otomatis | Dipicu saat mendarat di PPH atau PBM              |
| GADAI                 | Manual   | Gadaikan properti ke Bank                         |
| TEBUS                 | Manual   | Tebus properti yang digadai                       |
| BANGUN                | Manual   | Bangun rumah/hotel di street yang dimonopoli      |
| LELANG                | Otomatis | Dipicu saat tidak beli / bangkrut ke Bank         |
| FESTIVAL              | Otomatis | Dipicu saat mendarat di petak Festival            |
| BANGKRUT & AMBIL ALIH | Otomatis | Dipicu saat pemain tidak mampu bayar              |
| SIMPAN                | Manual   | Simpan state ke file .txt (hanya di awal giliran) |
| MUAT                  | Manual   | Load state dari file (hanya sebelum mulai)        |
| CETAK_LOG             | Manual   | Tampilkan log transaksi (semua atau N terakhir)   |
| MENANG                | Otomatis | Dipicu saat kondisi menang terpenuhi              |
| KESEMPATAN/DANA UMUM  | Otomatis | Dipicu saat mendarat di petak kartu               |
| GUNAKAN_KEMAMPUAN     | Manual   | Aktifkan kartu kemampuan spesial                  |
| DROP KARTU KEMAMPUAN  | Otomatis | Dipicu saat menerima kartu ke-4                   |

---

## 6. SAVE/LOAD FORMAT

```
<TURN_SAAT_INI> <MAX_TURN> <JUMLAH_PEMAIN>
<STATE_PEMAIN_1..N>
<URUTAN_GILIRAN_1..N>
<GILIRAN_AKTIF_SAAT_INI>
<STATE_PROPERTI>
<STATE_DECK>
<STATE_LOG>
```

**State Pemain:** `<USERNAME> <UANG> <POSISI> <STATUS> <JML_KARTU> [<JENIS> <NILAI> <DURASI>]...`

- STATUS: ACTIVE | BANKRUPT | JAILED

**State Properti:** `<JML_PROPERTI> [<KODE> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>]...`

- FMULT: 1/2/4/8 | FDUR: 0-3 | N_BANGUNAN: 0-4 atau H

**State Deck:** `<JML_KARTU_DECK> [<JENIS_KARTU>]...`

**State Log:** `<JML_ENTRI> [<TURN> <USERNAME> <JENIS_AKSI> <DETAIL>]...`

---

## 7. KETENTUAN TEKNIS

- **Bahasa**: C++11/14/17, Makefile, Linux
- **Wajib OOP**: Inheritance, Polymorphism, Exception Handling, Overloading, Abstract Class, Generic Class, STL (Vector + Map wajib)
- **Arsitektur**: 3 Layer — UI Layer, Game Logic Layer, Data Access Layer
- **Prinsip**: SOLID (S & L), DRY
- **Larangan**: struct, global variable berlebihan, God Class, dynamic_cast overuse, variant/any
- **Kelompok**: 5 orang

---

## 8. MILESTONE

| Milestone | Deadline      | Isi                          |
| --------- | ------------- | ---------------------------- |
| M1        | 12 April 2026 | Desain kelas (sudah lewat)   |
| M2        | 24 April 2026 | Implementasi penuh + laporan |

---

## 9. BONUS (Opsional)

- **GUI**: C++ (Raylib/SFML/imgui/wxWidget), tampilan saja
- **COM**: Computer Player yang bisa main mandiri
- **Papan Dinamis**: Baca konfigurasi papan dari file (20-60 petak, 1 GO, 1 Penjara)
- **Kreativitas**: Maks 1 fitur tambahan (Account + Leaderboard disarankan)
