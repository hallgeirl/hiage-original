#include <hiage/core/game.h>
#include <hiage/core/inputmanager.hpp>


class MarioGame : public hiage::Game
{
public:
	MarioGame(const hiage::KeyBindings& keyBindings) : hiage::Game(120, keyBindings) {}
	virtual ~MarioGame();

protected:
	virtual void onInit();
};

