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
		const Vector2<double>& getVelocity();
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

	class ComponentFactory
	{
	public:
		std::unique_ptr<Component> createComponent(const std::string& name) const;
	};
}