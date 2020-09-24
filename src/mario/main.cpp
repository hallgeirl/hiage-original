
#include <string>
#include <iostream>
#include <hiage/util/timer.h>
#include "mariogame.h"

using namespace hiage;
using namespace std;

int main(int argc, char* argv[])
{
	MarioGame game;
	Timer timer;
	timer.start();

	game.initialize(1024, 768, false);
	double frameTime = 0.005;

    game.scriptVM.runFile("data/scripts/objects.lua");
    game.scriptVM.runFile("data/scripts/ui.lua");
    game.scriptVM.runFile("data/scripts/initgame.lua");

	while (game.isRunning())
	{
        timer.reset();

        game.run(frameTime, true);
        timer.update();

        //cout << timer.getTime() << endl;
        frameTime = timer.getTime();

        //cap at 500 FPS
        while (frameTime < 0.002)
        {
            frameTime = timer.getTime();
        }

        if (frameTime > 0.020)
        {
            frameTime = 0.020;
        }
 	}
    
	return 0;
}
