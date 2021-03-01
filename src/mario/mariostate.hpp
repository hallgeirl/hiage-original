#include <hiage/gfx/fonts.hpp>

#include <hiage/core/game.hpp>
#include <hiage/core/map.hpp>

class MarioState : public hiage::MapState
{
private:
	int _score, _lives, _coins;
	hiage::Font _mainfont;
	int _fps;

public:
	MarioState(hiage::Game &game);
	virtual ~MarioState();
	virtual void init();
	virtual void destroy();

	virtual void pause();
	virtual void resume();

	virtual void handleEvents(double frametime);
	virtual void update(double frametime) override;
	virtual void render();
};

