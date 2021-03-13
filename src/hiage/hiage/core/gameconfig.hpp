#pragma once

#include "debugrenderer.hpp"


namespace hiage
{
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