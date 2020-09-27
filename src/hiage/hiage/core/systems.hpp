#pragma once

#include <string>
#include <memory>

namespace hiage
{
	class Game;
	class GameState;

	/*
		Systems
	*/
	class System
	{
	protected:
		Game& game;
		GameState& gameState;

	public:
		System(Game& game, GameState& gameState);
		virtual ~System();
		virtual void update(double frameTime) = 0;
	};

	class MovementSystem : public System
	{
	public:
		MovementSystem(Game& game, GameState& gameState);
		virtual void update(double frametime) override;
	};

	class ObjectRenderingSystem : public System
	{
	public:
		ObjectRenderingSystem(Game& game, GameState& gameState);
		virtual void update(double frameTime) override;
	};

	class GravitySystem : public System
	{
	private:
		double magnitude = 30;

	public:
		GravitySystem(Game& game, GameState& gameState);
		virtual void update(double frameTime) override;
	};


	// List of currently missing systems:
	// - Script system??
	// - Collision
	// - Object destruction?
	// - Controllers
	/*
		SystemsFactory
	*/

	class SystemsFactory
	{
	private:
		Game& game;
		GameState& gameState;
	public:
		SystemsFactory(Game& game, GameState& gameState);
		virtual std::unique_ptr<System> createSystem(std::string name);
	};
}