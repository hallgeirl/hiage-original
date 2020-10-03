#include <hiage/core/game.hpp>
#include <hiage/core/map.h>

class MarioState : public hiage::MapState
{
public:
	MarioState(hiage::Game &game);
	virtual ~MarioState();
	virtual void init();
	virtual void destroy();

	virtual void pause();
	virtual void resume();

	virtual void handleEvents(double frametime);
	virtual void update(double frametime) override;
	virtual void render(double frametime);
};

