#pragma once

#include <vector>
#include <string>
#include "../gfx/display.h"
#include "../gfx/fonts.hpp"

namespace hiage 
{
    struct QuadTreeDebugFlags
    {
        bool renderLeaves = false;
        bool drawChildCount = false;
    };
    struct DebugFlags
    {
        QuadTreeDebugFlags quadTreeDebugFlags;
        bool drawEntityInfo = false;
    };

    // Class for writing to text on-screen for debugging purposes
    class DebugRenderer
    {
    private:
        Display& _display;
        Font& _font;
        DebugFlags _debugFlags;
        bool _enabled = false;

    public:
        DebugRenderer(Display& display, Font& font) : _display(display), _font(font)
        {
        }

        void enable()
        {
            _enabled = true;
        }

        void disable()
        {
            _enabled = false;
        }

        void setDebugFlags(const DebugFlags& debugFlags)
        {
            _debugFlags = debugFlags;
        }

        const void renderText(const std::string& text, double x, double y)
        {   
            _font.renderText(_display.getRenderer(), text, Vector2<double>(x,y), 0.2);
        }

        DebugFlags& getDebugFlags()
        {
            return _debugFlags;
        }
    };
}