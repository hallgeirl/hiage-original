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

    struct DebugFlags
    {
        bool drawEntityInfo = false;
        QuadTreeDebugFlags quadTreeDebugFlags;
        UniformGridDebugFlags uniformGrid;
    };

    struct DebugConfig
    {
        bool enabled;
        DebugFlags debugFlags;
    };
    // A class that is used to pass in configuration to the game engine.
    struct GameConfig
    {
        // Debug flags
        DebugConfig debug;
        
        // Display cconfig
        int displayWidth, displayHeight;
        bool fullscreen;
        std::string consoleFontName;
    };
}