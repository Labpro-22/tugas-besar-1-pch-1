#ifndef PROPERTY_FACTORY_HPP
#define PROPERTY_FACTORY_HPP

#include <vector>
#include <memory>
#include <map>

#include "utils/ConfigLoader.hpp"
#include "../src/core/Property/Property.hpp"
#include "../src/core/Property/StreetProperty.hpp"
#include "../src/core/Property/RailroadProperty.hpp"
#include "../src/core/Property/UtilityProperty.hpp"

using namespace std;

class PropertyFactory
{
public:
    static vector<unique_ptr<Property>> createProperties(
        const vector<PropertyData> &propertyDataList,
        const map<int, int> &railroadRentMap,
        const map<int, int> &utilityFactorMap);
};

#endif