
#include <hiage/core/map.h>

#include <string>
#include <iostream>


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
    KeyBindings dummyKeyBindings;

    DummyGame g(dummyKeyBindings);
    g.initialize(1024, 768, false);
    DummyGamestate gs(g);
    Map m(g, gs);
    m.createFromFile("maps/mainmenu.map");
    
    m.saveAsJson("foo");
    return 0;
}
