
#include <hiage/core/map.h>

#include <string>
#include <iostream>

#include <base64/base64.hpp>

using namespace hiage;
using namespace std;

class DummyGame : public Game
{
public:
    DummyGame(const KeyBindings& keyBindings) : Game(120, keyBindings, DATAROOT) { }

    

    // Inherited via Game
    virtual void onInit() override
    {
        loadResources("textures", ResourceTypeEnum::TEXTURE);
        loadResources("sprites", ResourceTypeEnum::SPRITE);
        loadResources("tilesets", ResourceTypeEnum::TILESET);
        loadResources("objects", ResourceTypeEnum::OBJECT);
        loadResources("fonts", ResourceTypeEnum::FONT);
    }

};

class DummyGamestate : public GameState
{
public:
    DummyGamestate(Game& game) : GameState(game) {}


    // Inherited via GameState
    virtual void init() override
    {
    }

    virtual void destroy() override
    {
    }

    virtual void handleEvents(double) override
    {
    }

    virtual void render() override
    {
    }

};


int main(int, char*)
{
    std::vector<uint32_t> v;

    v.push_back(123);
    v.push_back(12938458);
    
    string result = macaron::Base64::Encode(v);

    KeyBindings dummyKeyBindings;

    DummyGame g(dummyKeyBindings);
    g.initialize(1024, 768, false);
    DummyGamestate gs(g);
    Map m(g, gs);

    vector<string> mapsToConvert = {
        "maps/deathmenu",
        "maps/gameover.map",
        "maps/level1.map",
        "maps/level2.map",
        "maps/mainmenu.map",
        "maps/testmap.map"
    };

    for (auto map : mapsToConvert)
    {
        m.createFromFile(map);
        auto dest = g.getResourcePath(map) + ".json";
        m.saveAsJson(dest);
    }
    

    return 0;
}
