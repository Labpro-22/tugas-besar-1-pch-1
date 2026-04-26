#ifndef CARD_FACTORY_HPP
#define CARD_FACTORY_HPP

#include "../Card/MoveCard.hpp"
#include "../Card/DiscountCard.hpp"
#include "../Card/ShieldCard.hpp"
#include "../Card/TeleportCard.hpp"
#include "../Card/LassoCard.hpp"
#include "../Card/DemolitionCard.hpp"
#include "../Card/FreeFromJailCard.hpp"

//  CardFactory
//  Buat SkillCard dari string type + nilai
//  Dipakai oleh SaveLoadManager saat load
//  Header-only karena simple static factory
class CardFactory
{
public:
    // value   = steps untuk MoveCard, discountPercent untuk DiscountCard
    // duration= sisa durasi untuk DiscountCard dan ShieldCard
    static SkillCard *createSkillCard(const std::string &type,
                                      double value = 0,
                                      int duration = 0)
    {
        if (type == "MoveCard")
            return new MoveCard((int)value);

        if (type == "DiscountCard")
            return new DiscountCard((int)value, duration);

        if (type == "ShieldCard")
            return new ShieldCard();

        if (type == "TeleportCard")
            return new TeleportCard();

        if (type == "LassoCard")
            return new LassoCard();

        if (type == "DemolitionCard")
            return new DemolitionCard();

        if (type == "FreeFromJailCard")
            return new FreeFromJailCard();

        return nullptr;
    }
};

#endif