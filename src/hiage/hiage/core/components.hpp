#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"

#include <memory>

namespace hiage 
{

	/*
	Components
	*/
	class Component
	{
	public:
		virtual ~Component() {}
	};

	class PhysicalComponent : public Component
	{
	private:
		Vector2<double>	position;   //current position
	public:
		void setPosition(double x, double y);
		const Vector2<double>& getPosition() const;
	};

	class MovableComponent : public Component
	{
	private:
		Vector2<double> velocity;      //current speed
	public:
		void setVelocity(double xVel, double yVel);
		const Vector2<double>& getVelocity() const;
		void accellerate(double magnitude, const Vector2<double>& directionNormalized);
	};

	class RenderableComponent : public Component
	{
	private:
		Sprite sprite;
	};

	/*
	ComponentFactory
	*/

	class ComponentManager
	{
	private: 
		std::map<std::string, std::vector<std::shared_ptr<Component>>> componentCache;
		std::unique_ptr<Component> createComponentCore(const std::string& type) const;
	public:
		// todo: after rewriting everything to ECS - see if we can drop to using unique_ptr.
		std::shared_ptr<Component> createComponent(const std::string& type);
		std::vector<std::shared_ptr<Component>> getComponentsOfType(const std::string& type);
	};
}