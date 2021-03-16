
#include <hiage/core/map.hpp>

#include <string>
#include <iostream>
#include <cassert>
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


bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

int main(int, char*)
{
    KeyBindings dummyKeyBindings;

    DummyGame g(dummyKeyBindings);
    GameConfig config;
    config.display.displayHeight = 768;
    config.display.displayWidth = 1024;
    config.display.fullscreen = false;

    g.initialize(config);
    DummyGamestate gs(g);
    Map m(g, gs);

    vector<string> mapsToConvert = {
        "maps/deathmenu.map",
        "maps/gameover.map",
        "maps/level1.map",
        "maps/level2.map",
        "maps/mainmenu.map",
        "maps/testmap.map"
    };

    for (auto map : mapsToConvert)
    {
        //m.createFromFile(map);
        
        std::string destFilename = map;
        replace(destFilename, ".map", ".json");
        assert(map != destFilename);

        auto dest = g.getResourcePath(destFilename);
        // Commented out to avoid accidental overwriting of map files
        // m.saveAsJson(dest); 
    }
    
    return 0;
}
