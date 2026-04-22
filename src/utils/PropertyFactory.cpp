#include "utils/PropertyFactory.hpp"
#include <stdexcept>

vector<unique_ptr<Property>> PropertyFactory::createProperties(
    const vector<PropertyData> &propertyDataList,
    const map<int, int> &railroadRentMap,
    const map<int, int> &utilityFactorMap)
{
    vector<unique_ptr<Property>> properties;

    for (const auto &p : propertyDataList)
    {
        if (p.type == "STREET")
        {
            map<int, int> rentMap;
            for (int i = 0; i < (int)p.rentLevels.size(); ++i)
            {
                rentMap[i] = p.rentLevels[i];
            }

            properties.push_back(make_unique<StreetProperty>(
                p.id, // kalau constructor property kamu masih string
                p.code,
                p.name,
                p.color,
                p.price,
                p.mortgage,
                "",
                p.upgHouse,
                p.upgHotel,
                rentMap,
                0,
                false,
                1,
                0));
        }
        else if (p.type == "RAILROAD")
        {
            properties.push_back(make_unique<RailroadProperty>(
                p.id,
                p.code,
                p.name,
                p.color,
                p.price,
                p.mortgage,
                "",
                railroadRentMap));
        }
        else if (p.type == "UTILITY")
        {
            properties.push_back(make_unique<UtilityProperty>(
                p.id,
                p.code,
                p.name,
                p.color,
                p.price,
                p.mortgage,
                "",
                utilityFactorMap));
        }
        else
        {
            throw runtime_error("Unknown property type: " + p.type);
        }
    }

    return properties;
}