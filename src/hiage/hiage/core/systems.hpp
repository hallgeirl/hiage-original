#pragma once

#include "collisions.hpp"

#include <flecs.h>
#include <string>
#include <memory>
#include <vector>

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
	public:
		System();
		virtual ~System();
		virtual void registerSystem(flecs::world& world) = 0;
	};

	class MovementSystem : public System
	{
	public:
		MovementSystem();
		virtual void registerSystem(flecs::world& world) override;
	};

	class ObjectRenderingSystem : public System
	{
	private:
		Game& _game;
	public:
		ObjectRenderingSystem(Game& game);
		virtual void registerSystem(flecs::world& world) override;
	};

	class PhysicsSystem : public System
	{
	private:
		double _gravity = 30;

	public:
		PhysicsSystem(double gravity);
		virtual void registerSystem(flecs::world& world) override;
	};

	class ControllerSystem : public System
	{
	private:
		Game& _game;
	public:
		ControllerSystem(Game& game);
		virtual void registerSystem(flecs::world& world) override;
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
		ObjectObjectCollisionDetectionSystem() : System() {}
		virtual void registerSystem(flecs::world& world) override;
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
		ObjectTileCollisionDetectionSystem(const Tilemap& tilemap) : System(), tilemap(tilemap) {}
		virtual void registerSystem(flecs::world& world) override;
	};


	/// <summary>
	/// Handles collisions between objects and tiles by blocking passage.
	/// In essence, it will handle object-tile collision events.
	/// </summary>
	class BlockingTileSystem : public System
	{
	public:
		BlockingTileSystem() : System() {}
		virtual void registerSystem(flecs::world& world) override;
	};

	class AnimationSystem : public System
	{
	public:
		AnimationSystem() : System() {}
		virtual void registerSystem(flecs::world& world) override;
	};

	// Used for e.g. camera tracking
	class ObjectTrackingSystem : public System
	{
	public:
		ObjectTrackingSystem() : System() {}
		virtual void registerSystem(flecs::world& world) override;
	};

	// Used for e.g. camera tracking
	class CameraSystem : public System
	{
	private:
		Game& _game;
	public:
		CameraSystem(Game& game) : System(), _game(game) {}
		virtual void registerSystem(flecs::world& world) override;
	};


	// List of currently missing systems:
	// - Script system??
	// - Collision
	// - Object destruction?
	// - Controllers
	/*
		SystemsFactory
	*/
	class SystemsManager
	{
	private:
		flecs::world& _ecs;
		std::vector<std::unique_ptr<System>> _systems; // We need to hold on to the instances of the registered systems because they may have references to objects like e.g. Game and GameState.
	public:
		SystemsManager(flecs::world& world) : _ecs(world) { }
		
		template<typename T, typename ...TRest>
		void registerSystem(TRest... args)
		{ 
			std::unique_ptr<T> sys = std::make_unique<T>(args...);
			sys->registerSystem(_ecs);
			_systems.push_back(std::move(sys)); // Store the system pointer
		};
	};
}