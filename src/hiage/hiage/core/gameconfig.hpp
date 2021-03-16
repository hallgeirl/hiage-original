#pragma once

namespace hiage
{
    struct QuadTreeDebugFlags
    {
        bool renderLeaves = false;
        bool drawChildCount = false;

        bool any()
        {
            return renderLeaves || drawChildCount;
        }
    };

    struct UniformGridDebugFlags
    {
        bool renderGrid = false;
        bool drawChildCount = false;

        bool any()
        {
            return renderGrid || drawChildCount;
        }
    };

    struct CollisionDetectionDebugFlags
    {
        bool showObjectCollisions = false;
        bool showTileCollisions = false;
        bool drawBoundingPolygon = false;
    };

    struct DebugFlags
    {
        bool drawEntityInfo = false;
        QuadTreeDebugFlags quadTreeDebugFlags;
        UniformGridDebugFlags uniformGrid;
        CollisionDetectionDebugFlags collisionDetection;
    };

    struct DebugConfig
    {
        bool enabled;
        DebugFlags debugFlags;
    };

    struct DisplayConfig
    {
        // Display config
        int displayWidth, displayHeight;
        bool fullscreen;
        bool vsync;
    };

    // A class that is used to pass in configuration to the game engine.
    struct GameConfig
    {
        // Debug flags
        DebugConfig debug;
        DisplayConfig display;

        std::string consoleFontName;
    };
}