#pragma once

#include <string>
#include <memory>

namespace hiage
{
	class ComponentManager;
	class EntityManager;
	class Game;
	class Renderer;

	class System
	{
	protected:
		ComponentManager& componentManager;
		EntityManager& entityManager;

	public:
		System(ComponentManager& componentManager, EntityManager& entityManager);
		virtual ~System();
		virtual void update(double frameTime) = 0;
	};

	class MovementSystem : public System
	{
	public:
		MovementSystem(ComponentManager& componentManager, EntityManager& entityManager);
		virtual void update(double frametime) override;
	};

	class ObjectRenderingSystem : public System
	{
	private:
		Renderer& renderer;
	public:
		ObjectRenderingSystem(ComponentManager& componentManager, EntityManager& entityManager, Renderer& renderer);
		virtual void update(double frameTime) override;
	};

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