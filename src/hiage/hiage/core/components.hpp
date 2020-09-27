#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"

#include <memory>

namespace hiage 
{
	class Game;

	/*
	Components
	*/
	class Component
	{
	private:
		int typeId;
	protected:
		Component(int typeId);
	public:
		virtual ~Component();
		int getTypeId();
	};

#define NOARG
#define DEFINE_COMPONENT(NAME, PRIVATE, PUBLIC, _TYPEID) \
class NAME : public Component \
{\
private: \
	PRIVATE \
public: \
	NAME() : Component(NAME::TYPEID) {} \
	static const int TYPEID = _TYPEID; \
	PUBLIC \
}

	class PhysicalComponent : public Component
	{
	private:
		Vector2<double>	position;   //current position
	public:
		PhysicalComponent();
		PhysicalComponent(double x, double y);

		static const int TYPEID = 1;
		void setPosition(double x, double y);
		void setPosition(const Vector2<double> pos);
		const Vector2<double>& getPosition() const;
	};

	class MovableComponent : public Component
	{
	private:
		Vector2<double> velocity;      //current speed
	public:
		MovableComponent();
		MovableComponent(double velX, double velY);

		static const int TYPEID = 2;
		void setVelocity(double xVel, double yVel);
		void setVelocity(const Vector2<double> vel);
		const Vector2<double>& getVelocity() const;
		void accellerate(double magnitude, const Vector2<double>& directionNormalized);
	};

	class RenderableComponent : public Component
	{
	private:
		Sprite sprite;
	public:
		RenderableComponent(const Sprite& sprite);

		Sprite& getSprite();

		static const int TYPEID = 3;
	};


	//DEFINE_COMPONENT(GravityComponent, NOARG, NOARG, 4);

	class PhysicsComponent : public Component
	{
	private:
	public:
		PhysicsComponent();
		
		static const int TYPEID = 4;
	};

	class HumanControllerComponent : public Component
	{
	private:
	public:
		HumanControllerComponent();

		static const int TYPEID = 5;
	};

	/*
	ComponentManager
	*/

	class ComponentManager
	{
	private: 
		std::map<std::string, std::vector<std::shared_ptr<Component>>> componentCache;
		// TODO - Make it possible to register new component factory classes
		std::unique_ptr<Component> createComponentCore(const std::string& type, const std::map<std::string, std::string>& attributes, const std::map<std::string, void*>& runtimeAttributes);
		std::unique_ptr<Component> createRenderable(const std::map<std::string, std::string>& attributes);

		Game& game;
	public:
		ComponentManager(Game& game);
		~ComponentManager();
		// todo: after rewriting everything to ECS - see if we can drop to using unique_ptr.
		std::shared_ptr<Component> createComponent(const std::string& type, const std::map<std::string, std::string>& attributes, const std::map<std::string, void*>& runtimeAttributes);
		std::vector<std::shared_ptr<Component>> getComponentsOfType(const std::string& type);
	};
}