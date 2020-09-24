#include <hiage/core/game.h>


class MarioGame : public hiage::Game
{
public:
	virtual ~MarioGame();

protected:
	virtual void onInit();
};

