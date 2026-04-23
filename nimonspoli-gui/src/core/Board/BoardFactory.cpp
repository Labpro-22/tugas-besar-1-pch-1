#include "BoardFactory.hpp"

BoardFactory::BoardFactory(){}
Board* BoardFactory::createBoard(const vector<PropertyData> &propertyDataList, const vector<ActionData> &actionDataList, SpecialConfig &config, CardDeck<Card> *chanceCard, CardDeck<Card> *genFundCard, const std::vector<std::unique_ptr<Property>> &properties)
{
    std::map<int, Tile *> tileMap;
    for (const auto &a : actionDataList)
    {
        Tile *t = nullptr;
        if(a.code == "GO"){
            t = new GoTile(a.id, a.color, TileType::SPECIAL, a.name ,a.code, config.goSalary);
        }
        else if(a.code == "DNU"){
            t = new CardTile(a.id, a.color, TileType::CARD, a.name, a.code, chanceCard);
        }
        else if(a.code == "KSP"){
            t = new CardTile(a.id, a.color, TileType::CARD, a.name, a.code, genFundCard);
        }
        else if(a.code == "FES"){
            t = new FestivalTile(a.id, a.color, TileType::FESTIVAL,a.name, a.code);
        }
        else if(a.code=="PEN"){
            t = new JailTile(a.id, a.color, TileType::SPECIAL, a.name, a.code, {}, {}, config.jailFine);
        }
        else if(a.code == "BBP"){
            t = new FreeParkingTile(a.id, a.color, TileType::SPECIAL, a.name, a.code);
        }
        else if(a.code=="PPJ"){
            t = new GoToJail(a.id, a.color, TileType::SPECIAL, a.name, a.code);
        }
        else if (a.type == "TAX")
        {
            t = new TaxTile(a.id, a.color, TileType::TAX, a.name, a.code);
        }
        if(t) tileMap[a.id] = t;
    }

    for(const auto& a:propertyDataList ){

        Property *linkedProp = nullptr;
        for (auto &p : properties)
        {
            if (p->getCode() == a.code)
            {
                linkedProp = p.get(); 
                break;
            }
        }
        Tile *t = nullptr;
        if(a.type == "STREET"){
            auto *st = new StreetTile(a.id, a.color, TileType::STREET, a.name, a.code);
            st->setProperty(linkedProp); 
            t = st;
        }
        else if(a.type =="RAILROAD"){
            auto *rt = new RailRoadTile(a.id, a.color, TileType::RAILROAD, a.name, a.code);
            rt->setProperty(linkedProp);
            t = rt;
        }
        else if (a.type == "UTILITY")
        {
            auto *ut = new UtilityTile(a.id, a.color, TileType::UTILITY, a.name, a.code);
            ut->setProperty(linkedProp);
            t = ut;
        }

        if(t) tileMap[a.id] = t;
    }

    vector<Tile *> allTiles;
    for(auto& [id, tile]: tileMap){
        allTiles.push_back(tile);
    }
    return new Board(allTiles, allTiles.size());
}