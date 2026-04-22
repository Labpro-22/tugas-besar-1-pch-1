#include <iostream>
#include <string>
#include <vector>
#include "../src/core/Player/Player.hpp"
using namespace std;

static void printLine()
{
    cout << "==================================================\n";
}

static string statusToString(PlayerStatus status)
{
    switch (status)
    {
    case PlayerStatus::ACTIVE:
        return "ACTIVE";
    case PlayerStatus::JAILED:
        return "JAILED";
    case PlayerStatus::BANKRUPT:
        return "BANKRUPT";
    default:
        return "UNKNOWN";
    }
}

int main()
{
    printLine();
    cout << "TEST PLAYER\n";
    printLine();

    bool allPassed = true;

    Player p1("Widya", 1000);
    Player p2("Nicha", 700);

    cout << "[1] Constructor & Getter\n";
    cout << "Username   : " << p1.getUsername() << "\n";
    cout << "Balance    : " << p1.getBalance() << "\n";
    cout << "Position   : " << p1.getPosition() << "\n";
    cout << "Status     : " << statusToString(p1.getStatus()) << "\n";
    cout << "JailTurns  : " << p1.getJailTurns() << "\n\n";

    if (p1.getUsername() != "Widya")
    {
        cout << "FAIL: username salah\n";
        allPassed = false;
    }

    if (p1.getBalance() != 1000)
    {
        cout << "FAIL: balance awal salah\n";
        allPassed = false;
    }

    cout << "[2] setPosition, setStatus, setJailTurns\n";
    p1.setPosition(12);
    p1.setStatus(PlayerStatus::JAILED);
    p1.setJailTurns(2);

    cout << "Position   : " << p1.getPosition() << "\n";
    cout << "Status     : " << statusToString(p1.getStatus()) << "\n";
    cout << "JailTurns  : " << p1.getJailTurns() << "\n\n";

    if (p1.getPosition() != 12)
    {
        cout << "FAIL: setPosition gagal\n";
        allPassed = false;
    }

    if (p1.getStatus() != PlayerStatus::JAILED)
    {
        cout << "FAIL: setStatus gagal\n";
        allPassed = false;
    }

    if (p1.getJailTurns() != 2)
    {
        cout << "FAIL: setJailTurns gagal\n";
        allPassed = false;
    }

    cout << "[3] operator += dan -=\n";
    p1 += 500;
    cout << "Setelah += 500, balance = " << p1.getBalance() << "\n";
    if (p1.getBalance() != 1500)
    {
        cout << "FAIL: operator += gagal\n";
        allPassed = false;
    }

    p1 -= 200;
    cout << "Setelah -= 200, balance = " << p1.getBalance() << "\n\n";
    if (p1.getBalance() != 1300)
    {
        cout << "FAIL: operator -= gagal\n";
        allPassed = false;
    }

    cout << "[4] canAfford\n";
    cout << "canAfford(1000): " << (p1.canAfford(1000) ? "true" : "false") << "\n";
    cout << "canAfford(2000): " << (p1.canAfford(2000) ? "true" : "false") << "\n\n";

    if (!p1.canAfford(1000))
    {
        cout << "FAIL: canAfford(1000) harus true\n";
        allPassed = false;
    }

    if (p1.canAfford(2000))
    {
        cout << "FAIL: canAfford(2000) harus false\n";
        allPassed = false;
    }

    cout << "[5] operator > dan <\n";
    cout << "p1.balance = " << p1.getBalance() << ", p2.balance = " << p2.getBalance() << "\n";
    cout << "(p1 > p2): " << ((p1 > p2) ? "true" : "false") << "\n";
    cout << "(p2 < p1): " << ((p2 < p1) ? "true" : "false") << "\n\n";

    if (!(p1 > p2))
    {
        cout << "FAIL: operator > salah\n";
        allPassed = false;
    }

    if (!(p2 < p1))
    {
        cout << "FAIL: operator < salah\n";
        allPassed = false;
    }

    cout << "[6] Jail logic\n";
    p1.goToJail();
    cout << "Setelah goToJail -> Status: " << statusToString(p1.getStatus())
         << ", JailTurns: " << p1.getJailTurns() << "\n";

    if (!p1.isInJail())
    {
        cout << "FAIL: isInJail harus true setelah goToJail\n";
        allPassed = false;
    }

    p1.incrementJailTurns();
    cout << "Setelah incrementJailTurns -> JailTurns: " << p1.getJailTurns() << "\n";

    if (p1.getJailTurns() != 1)
    {
        cout << "FAIL: incrementJailTurns gagal\n";
        allPassed = false;
    }

    p1.releaseFromJail();
    cout << "Setelah releaseFromJail -> Status: " << statusToString(p1.getStatus())
         << ", JailTurns: " << p1.getJailTurns() << "\n\n";

    if (p1.isInJail())
    {
        cout << "FAIL: releaseFromJail gagal\n";
        allPassed = false;
    }

    cout << "[7] Shield logic\n";
    p1.activateShield();
    cout << "Shield aktif? " << (p1.isShielded() ? "true" : "false") << "\n";

    if (!p1.isShielded())
    {
        cout << "FAIL: activateShield gagal\n";
        allPassed = false;
    }

    p1.deactivateShield();
    cout << "Shield aktif setelah deactivate? " << (p1.isShielded() ? "true" : "false") << "\n\n";

    if (p1.isShielded())
    {
        cout << "FAIL: deactivateShield gagal\n";
        allPassed = false;
    }

    cout << "[8] onTurnStart\n";
    p1.activateShield();
    p1.onTurnStart();
    cout << "Shield setelah onTurnStart? " << (p1.isShielded() ? "true" : "false") << "\n\n";

    if (p1.isShielded())
    {
        cout << "FAIL: onTurnStart seharusnya mematikan shield\n";
        allPassed = false;
    }

    cout << "[9] Property container test\n";
    cout << "Catatan: test ini hanya cek add/remove/count pointer property.\n";

    // Karena Property abstract, kita tidak instantiate object asli di sini.
    // Kita hanya pakai dummy pointer null untuk cek perilaku vector.
    Property *dummy1 = nullptr;
    Property *dummy2 = reinterpret_cast<Property *>(0x1);

    p1.addProperty(dummy1);
    p1.addProperty(dummy2);

    cout << "Property count setelah add 2 item: " << p1.getPropertyCount() << "\n";
    if (p1.getPropertyCount() != 2)
    {
        cout << "FAIL: addProperty/getPropertyCount gagal\n";
        allPassed = false;
    }

    p1.removeProperty(dummy1);
    cout << "Property count setelah remove 1 item: " << p1.getPropertyCount() << "\n";
    if (p1.getPropertyCount() != 1)
    {
        cout << "FAIL: removeProperty gagal\n";
        allPassed = false;
    }

    p1.removeProperty(dummy2);
    cout << "Property count setelah remove semua: " << p1.getPropertyCount() << "\n\n";
    if (p1.getPropertyCount() != 0)
    {
        cout << "FAIL: removeProperty kedua gagal\n";
        allPassed = false;
    }

    printLine();
    if (allPassed)
    {
        cout << "HASIL AKHIR: SEMUA TEST PLAYER LOLOS\n";
    }
    else
    {
        cout << "HASIL AKHIR: ADA TEST PLAYER YANG GAGAL\n";
    }
    printLine();

    return allPassed ? 0 : 1;
}