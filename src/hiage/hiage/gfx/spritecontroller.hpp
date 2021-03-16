#pragma once

#include "renderer.h"
#include "sprite.h"
#include "../util/vector2.h"

namespace hiage 
{
    class Game;
    /*
    Class for doing everything related to sprites -- rendering, controlling animations, etc.
    This way, the sprite object itself can remain a pure data object.
    */
    class SpriteController
    {
    private:
        Game& _game;
    public:
        SpriteController(Game& game) : _game(game) {}
        void render(const Sprite& sprite, const Vector2<double>& position, ObjectZ z, float rotation = 0, bool hFlip = false, bool vFlip = false, float maxSize = 0) const;
    };
}