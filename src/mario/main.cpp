
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
	game.initialize(1920, 1080, false);

    game.scriptVM.runFile("scripts/objects.lua");
    game.scriptVM.runFile("scripts/ui.lua");
    game.scriptVM.runFile("scripts/initgame.lua");

	while (game.isRunning())
	{
        game.run(true);
 	}
    
	return 0;
}
