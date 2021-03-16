
#include <string>
#include <iostream>
#include <hiage/util/timer.h>
#include "mariogame.hpp"
#include <chrono>
#include <thread>


using namespace hiage;
using namespace std;

int main(int, char*)
{
    KeyBindings keyBindings;
    keyBindings.mapKey("left", "goLeft");
    keyBindings.mapKey("right", "goRight");
    keyBindings.mapKey("up", "lookUp");
    keyBindings.mapKey("down", "crouch");
    keyBindings.mapKey("space", "jump");
    keyBindings.mapKey("lshift", "run");
    keyBindings.mapKey("rshift", "run");

	MarioGame game(keyBindings);
    GameConfig config;
    config.display.displayHeight = 1080;
    config.display.displayWidth = 1920;
    config.display.fullscreen = false;
    config.display.vsync = true;
    config.consoleFontName = "SmallFont";
    config.debug = DebugConfig {
        .enabled = false,
        .debugFlags = {
            .drawEntityInfo = true,
            .quadTreeDebugFlags = QuadTreeDebugFlags {
                .renderLeaves = true, 
                .drawChildCount = true
            },
            .uniformGrid = UniformGridDebugFlags {
                .renderGrid = true,
                .drawChildCount = true
            }
        }
    };
	game.initialize(config);

    game.scriptVM.runFile("scripts/objects.lua");
    game.scriptVM.runFile("scripts/ui.lua");
    game.scriptVM.runFile("scripts/initgame.lua");

	while (game.isRunning())
	{
        game.run(true);
 	}
    
	return 0;
}
