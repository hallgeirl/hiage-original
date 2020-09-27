#pragma once

#include <string>
#include <memory>

namespace hiage
{
	class ComponentManager;
	class EntityManager;
	class Game;
	class Renderer;

	/*
		Systems
	*/
	class System
	{
	protected:
		ComponentManager& componentManager;
		EntityManager& entityManager;
		Game& game;

	public:
		System(Game& game, ComponentManager& componentManager, EntityManager& entityManager);
		virtual ~System();
		virtual void update(double frameTime) = 0;
	};

	class MovementSystem : public System
	{
	public:
		MovementSystem(Game& game, ComponentManager& componentManager, EntityManager& entityManager);
		virtual void update(double frametime) override;
	};

	class ObjectRenderingSystem : public System
	{
	private:
		Renderer& renderer;
	public:
		ObjectRenderingSystem(Game& game, ComponentManager& componentManager, EntityManager& entityManager, Renderer& renderer);
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
		ComponentManager& componentManager;
		EntityManager& entityManager;
	public:
		SystemsFactory(ComponentManager& componentFactory, EntityManager& entityManager, Game& game);
		virtual std::unique_ptr<System> createSystem(std::string name);
	};
}