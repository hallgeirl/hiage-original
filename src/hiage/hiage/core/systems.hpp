#pragma once

#include "collisions.hpp"

#include <string>
#include <memory>

namespace hiage
{
	class Game;
	class GameState;
	class Tilemap;

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

	class HumanControllerSystem : public System
	{
	public:
		HumanControllerSystem(Game& game, GameState& gameState);
		virtual void update(double frameTime) override;
	};

	/// <summary>
	/// Detects collisions between two objects. 
	/// For detected collisions, put an event on the gamestate's event queue.
	/// </summary>
	class ObjectObjectCollisionDetectionSystem : public System
	{
	private:
		SATCollisionTester collisionTester;

	public:
		ObjectObjectCollisionDetectionSystem(Game& game, GameState& gameState) : System(game, gameState) {}
		virtual void update(double frameTime) override;
	};

	/// <summary>
	/// Detects collisions between an object and a tile. 
	/// For detected collisions, put an event on the gamestate's event queue.
	/// </summary>
	class ObjectTileCollisionDetectionSystem : public System
	{
	private:
		const Tilemap& tilemap;
		SATCollisionTester collisionTester;

	public:
		ObjectTileCollisionDetectionSystem(Game& game, GameState& gameState, const Tilemap& tilemap) : System(game, gameState), tilemap(tilemap) {}
		virtual void update(double frameTime) override;
	};


	/// <summary>
	/// Handles collisions between objects and tiles by blocking passage.
	/// In essence, it will handle object-tile collision events.
	/// </summary>
	class BlockingTileSystem : public System
	{
	public:
		BlockingTileSystem(Game& game, GameState& gameState) : System(game, gameState) {}
		virtual void update(double frameTime) override;
	};

	class AnimationSystem : public System
	{
	public:
		AnimationSystem(Game& game, GameState& gameState) : System(game, gameState) {}
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
		
		template<typename T, typename ...TRest>
		std::unique_ptr<System> createSystem(TRest... args)
		{ 
			return make_unique<T>(game, gameState, args...);
		};
	};
}