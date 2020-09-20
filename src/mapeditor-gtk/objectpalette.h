#pragma once

#include <vector>
#include <string>

#include <engine/com_game.h>
#include <engine/util_fonts.h>
#include <engine/game_entity.h>


class ObjectPalette
{
private:
    bool visible, initialized;
    std::vector<Engine::PhysicalEntity *> objects;

    float width, height;
    int displaySize;
    Engine::Vector2<double> position;

public:
    ObjectPalette();
    ~ObjectPalette();

    bool create(Engine::Game * game, float width, float height, Engine::Vector2<double> position, int displaySize);

    int getObjectFromInput(Engine::Game *game, Engine::Vector2<double> mousePosition);
    std::string getObjectName(int index);
    void render(Engine::Game * game, Engine::Font * font);

    void hide();
    void show();
};
