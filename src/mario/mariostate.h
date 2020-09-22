#include <hiage/core/game.h>
#include <hiage/core/entity.h>
#include <hiage/core/map.h>


class MarioState : public Hinage::MapState
{
public:
	MarioState(Hinage::Game &game);
	virtual ~MarioState();
	virtual void init();
	virtual void destroy();

	virtual void pause();
	virtual void resume();

	virtual void handleEvents(double frametime);
	virtual void update(double frametime);
	virtual void render(double frametime);
};

