#pragma once

#include <string>
#include <memory>

namespace hiage
{
	class ComponentManager;

	class System
	{
	protected:
		ComponentManager& componentManager;

	public:
		System(ComponentManager& componentManager);
		virtual ~System();
		virtual void update(double frameTime) = 0;
	};

	class MovementSystem : public System
	{
	public:
		MovementSystem(ComponentManager& componentManager);
		virtual void update(double frametime) override;
	};

	class ObjectRenderingSystem : public System
	{
	public:
		ObjectRenderingSystem(ComponentManager& componentManager);
		virtual void update(double frameTime) override;
	};

	class SystemsFactory
	{
	private:
		ComponentManager& componentManager;
	public:
		SystemsFactory(ComponentManager& componentFactory);
		virtual std::unique_ptr<System> createSystem(std::string name);
	};
}