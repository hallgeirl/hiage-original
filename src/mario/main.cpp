
#include <string>
#include <iostream>
#include <hiage/util/timer.h>
#include "mariogame.hpp"
#include <chrono>
#include <thread>


using namespace hiage;
using namespace std;

int main(int argc, char* argv[])
{
    KeyBindings keyBindings;
    keyBindings.mapKey("left", "goLeft");
    keyBindings.mapKey("right", "goRight");
    keyBindings.mapKey("up", "lookUp");
    keyBindings.mapKey("down", "crouch");
    keyBindings.mapKey("space", "jump");
    keyBindings.mapKey("lshift", "run");

	MarioGame game(keyBindings);
	game.initialize(1024, 768, false);

    game.scriptVM.runFile("data/scripts/objects.lua");
    game.scriptVM.runFile("data/scripts/ui.lua");
    game.scriptVM.runFile("data/scripts/initgame.lua");

    double frameTimeLimitMicroseconds = 1000000. / 120.;

	while (game.isRunning())
	{
        game.run(true);
 	}
    
	return 0;
}
