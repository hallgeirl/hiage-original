#include <hiage/core/game.h>


class MarioGame : public hiage::Game
{
public:
	MarioGame() : hiage::Game(120) {}
	virtual ~MarioGame();

protected:
	virtual void onInit();
};

