#pragma once

#include <vector>
#include <string>
#include "../gfx/display.h"

namespace hiage 
{
    // Class for writing to text on-screen for debugging purposes
    class DebugWriter
    {
    private:
        std::vector<std::string> _onScreenBuffer;
        Display& _display;
    public:
        DebugWriter(Display& display) : _display(display)
        {
        }
        void reset()
        {
            _onScreenBuffer.clear();
        }
        
        void write(const std::string& line)
        {
            _onScreenBuffer.push_back(line);
        }

        const std::vector<std::string>& getBuffer()
        {
            return _onScreenBuffer;
        }
    };
}