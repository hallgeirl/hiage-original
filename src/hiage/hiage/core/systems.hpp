#pragma once

namespace hiage
{
	class Game;

	class System
	{
	protected:
		const Game& game;

	public:
		System(const Game& game);
		virtual ~System();
		virtual void invoke(double frameTime) = 0;
	};

	class MovementSystem
	{
	public:
		virtual void invoke(double frametime);
	};

	class ObjectRenderingSystem : public System
	{
	public:
		virtual void invoke(double frameTime) override;
	};
}