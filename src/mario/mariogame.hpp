#include <hiage/core/game.hpp>
#include <hiage/core/inputmanager.hpp>


class MarioGame : public hiage::Game
{
public:
#ifdef DATAROOT
	MarioGame(const hiage::KeyBindings& keyBindings) : hiage::Game(1200, keyBindings, DATAROOT) {}
#else
	MarioGame(const hiage::KeyBindings& keyBindings) : hiage::Game(120, keyBindings, "data") {}
#endif
	virtual ~MarioGame();

protected:
	virtual void onInit();
};

