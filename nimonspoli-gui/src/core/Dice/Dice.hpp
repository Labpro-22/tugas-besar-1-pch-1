#ifndef DICE_HPP
#define DICE_HPP

class Dice {
private:
    int daduVal1;
    int daduVal2;
    int playerConcecutiveDoubles; // Penamaan atribut disesuaikan dengan diagram kelas

public:
    // Konstruktor
    Dice();

    // Menghasilkan angka acak untuk daduVal1 dan daduVal2
    void rollRandom();

    // Mengatur angka secara manual untuk kebutuhan testing spesifik
    void setManual(int v1, int v2);

    // Mengembalikan total nilai kedua dadu
    int getTotal() const;

    // Mengembalikan berapa kali pemain mendapatkan double berturut-turut
    int getConsecutiveDoubles() const;

    // Mereset counter double berturut-turut (misal saat pemain ganti giliran atau masuk penjara)
    void resetConsecutiveDoubles(); // Penamaan method disesuaikan dengan typo di UML diagram

    // Mengembalikan true jika kedua dadu bernilai sama
    bool isDouble() const;

    // Tambahan getter opsional yang direkomendasikan untuk mempermudah logging/display
    int getDaduVal1() const;
    int getDaduVal2() const;
};

#endif