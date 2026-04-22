#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

#include "../include/utils/ConfigLoader.hpp"
#include "../include/utils/PropertyFactory.hpp"
#include "../src/core/Property/Property.hpp"
#include "../src/core/Property/StreetProperty.hpp"
#include "../src/core/Property/RailroadProperty.hpp"
#include "../src/core/Property/UtilityProperty.hpp"

using namespace std;

static string typeOf(const Property *p)
{
    if (dynamic_cast<const StreetProperty *>(p))
        return "STREET";
    if (dynamic_cast<const RailroadProperty *>(p))
        return "RAILROAD";
    if (dynamic_cast<const UtilityProperty *>(p))
        return "UTILITY";
    return "UNKNOWN";
}

static void printDivider()
{
    cout << "============================================================\n";
}

int main()
{
    try
    {
        printDivider();
        cout << "TEST PROPERTY FACTORY\n";
        printDivider();

        ConfigLoader loader("config");

        vector<PropertyData> rawProperties = loader.loadProperties();
        map<int, int> railroadMap = loader.loadRailroad();
        map<int, int> utilityMap = loader.loadUtility();

        cout << "[STEP 1] Raw config loaded\n";
        cout << "Jumlah raw property   : " << rawProperties.size() << "\n";
        cout << "Railroad config size  : " << railroadMap.size() << "\n";
        cout << "Utility config size   : " << utilityMap.size() << "\n\n";

        vector<unique_ptr<Property>> properties =
            PropertyFactory::createProperties(rawProperties, railroadMap, utilityMap);

        cout << "[STEP 2] Factory executed\n";
        cout << "Jumlah object property: " << properties.size() << "\n\n";

        if (properties.size() != rawProperties.size())
        {
            cerr << "[FAIL] Jumlah object hasil factory tidak sama dengan raw property\n";
            return 1;
        }

        int streetCount = 0;
        int railroadCount = 0;
        int utilityCount = 0;
        int unknownCount = 0;

        bool mismatchFound = false;

        for (size_t i = 0; i < properties.size(); ++i)
        {
            string expectedType = rawProperties[i].type;
            string actualType = typeOf(properties[i].get());

            if (actualType == "STREET")
                streetCount++;
            else if (actualType == "RAILROAD")
                railroadCount++;
            else if (actualType == "UTILITY")
                utilityCount++;
            else
                unknownCount++;

            if (expectedType != actualType)
            {
                mismatchFound = true;
                cerr << "[FAIL] Type mismatch pada index " << i
                     << " | expected=" << expectedType
                     << " actual=" << actualType << "\n";
            }

            if (properties[i]->getCode() != rawProperties[i].code)
            {
                mismatchFound = true;
                cerr << "[FAIL] Code mismatch pada index " << i
                     << " | expected=" << rawProperties[i].code
                     << " actual=" << properties[i]->getCode() << "\n";
            }

            if (properties[i]->getName() != rawProperties[i].name)
            {
                mismatchFound = true;
                cerr << "[FAIL] Name mismatch pada index " << i
                     << " | expected=" << rawProperties[i].name
                     << " actual=" << properties[i]->getName() << "\n";
            }

            if (properties[i]->getColorGroup() != rawProperties[i].color)
            {
                mismatchFound = true;
                cerr << "[FAIL] Color mismatch pada index " << i
                     << " | expected=" << rawProperties[i].color
                     << " actual=" << properties[i]->getColorGroup() << "\n";
            }

            if (properties[i]->getPurchasePrice() != (int)rawProperties[i].price)
            {
                mismatchFound = true;
                cerr << "[FAIL] Price mismatch pada index " << i
                     << " | expected=" << (int)rawProperties[i].price
                     << " actual=" << properties[i]->getPurchasePrice() << "\n";
            }

            if (properties[i]->getMortageValue() != (int)rawProperties[i].mortgage)
            {
                mismatchFound = true;
                cerr << "[FAIL] Mortgage mismatch pada index " << i
                     << " | expected=" << (int)rawProperties[i].mortgage
                     << " actual=" << properties[i]->getMortageValue() << "\n";
            }
        }

        cout << "[STEP 3] Ringkasan tipe object\n";
        cout << "Street   : " << streetCount << "\n";
        cout << "Railroad : " << railroadCount << "\n";
        cout << "Utility  : " << utilityCount << "\n";
        cout << "Unknown  : " << unknownCount << "\n\n";

        cout << "[STEP 4] Sample output 10 property pertama\n";
        cout << left
             << setw(5) << "No"
             << setw(8) << "ID"
             << setw(8) << "Code"
             << setw(24) << "Name"
             << setw(14) << "Type"
             << setw(14) << "Color"
             << setw(8) << "Price"
             << setw(10) << "Mortgage"
             << "\n";

        int limit = min((int)properties.size(), 10);
        for (int i = 0; i < limit; ++i)
        {
            cout << left
                 << setw(5) << i
                 << setw(8) << properties[i]->getId()
                 << setw(8) << properties[i]->getCode()
                 << setw(24) << properties[i]->getName()
                 << setw(14) << typeOf(properties[i].get())
                 << setw(14) << properties[i]->getColorGroup()
                 << setw(8) << properties[i]->getPurchasePrice()
                 << setw(10) << properties[i]->getMortageValue()
                 << "\n";
        }
        cout << "\n";

        cout << "[STEP 5] Smoke test method khusus turunan\n";

        for (const auto &p : properties)
        {
            if (auto s = dynamic_cast<StreetProperty *>(p.get()))
            {
                cout << "Street sample   : " << s->getCode()
                     << " | rent(no monopoly)=" << s->calculateRentPrice(0, 0, false)
                     << " | sellPrice=" << s->calculateSellPrice() << "\n";
                break;
            }
        }

        for (const auto &p : properties)
        {
            if (auto r = dynamic_cast<RailroadProperty *>(p.get()))
            {
                cout << "Railroad sample : " << r->getCode()
                     << " | rent(1 railroad)=" << r->calculateRentPrice(0, 1, false)
                     << " | sellPrice=" << r->calculateSellPrice() << "\n";
                break;
            }
        }

        for (const auto &p : properties)
        {
            if (auto u = dynamic_cast<UtilityProperty *>(p.get()))
            {
                cout << "Utility sample  : " << u->getCode()
                     << " | rent(dice=7, count=1)=" << u->calculateRentPrice(7, 1, false)
                     << " | sellPrice=" << u->calculateSellPrice() << "\n";
                break;
            }
        }

        cout << "\n";

        if (mismatchFound || unknownCount > 0)
        {
            cerr << "[RESULT] TEST GAGAL: ada mismatch antara raw config dan object hasil factory.\n";
            return 1;
        }

        cout << "[RESULT] TEST BERHASIL: PropertyFactory berhasil membuat object sesuai config.\n";
        printDivider();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << "[EXCEPTION] " << e.what() << "\n";
        return 1;
    }
}