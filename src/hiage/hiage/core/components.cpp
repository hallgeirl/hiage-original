#include "components.hpp"
#include "game.h"
#include <memory>

using namespace hiage;
using namespace std;


hiage::Component::Component(int typeId) : typeId(typeId)
{
}

hiage::Component::~Component()
{
}

int hiage::Component::getTypeId()
{
	return typeId;
}

hiage::PhysicalComponent::PhysicalComponent() : Component(PhysicalComponent::TYPEID)
{

}

hiage::PhysicalComponent::PhysicalComponent(double x, double y) : Component(PhysicalComponent::TYPEID), position(x,y)
{
}

void PhysicalComponent::setPosition(double x, double y)
{
	position.set(x, y);
}

void hiage::PhysicalComponent::setPosition(const Vector2<double> pos)
{
	position.set(pos);
}

const Vector2<double>& PhysicalComponent::getPosition() const
{
	return position;
}

hiage::MovableComponent::MovableComponent() : Component(MovableComponent::TYPEID)
{
}

hiage::MovableComponent::MovableComponent(double velX, double velY) : Component(MovableComponent::TYPEID), velocity(velX, velY)
{
}

void MovableComponent::setVelocity(double xVel, double yVel)
{
	velocity.set(xVel, yVel);
}

void hiage::MovableComponent::setVelocity(const Vector2<double> vel)
{
	velocity.set(vel);
}

const Vector2<double>& MovableComponent::getVelocity() const
{
	return velocity;
}

void MovableComponent::accellerate(double magnitude, const Vector2<double>& directionNormalized)
{
	velocity += directionNormalized * magnitude;
}


hiage::RenderableComponent::RenderableComponent(const Sprite& sprite) : Component(RenderableComponent::TYPEID), sprite(sprite)
{

}

Sprite& hiage::RenderableComponent::getSprite()
{
	return sprite;
}

/*
	ComponentManager
*/


std::unique_ptr<Component> hiage::ComponentManager::createComponentCore(const std::string& name, const std::map<std::string, std::string>& attributes, const std::map<std::string, void*>& runtimeAttributes)
{
	if (name == "physical")
	{
		double x = 0, y = 0;

		if (runtimeAttributes.find("x") != runtimeAttributes.end())
			x = *(double*)(runtimeAttributes.at("x"));

		if (runtimeAttributes.find("y") != runtimeAttributes.end())
			y = *(double*)(runtimeAttributes.at("y"));

		return make_unique<PhysicalComponent>(x, y);
	}
	else if (name == "movable")
	{
		return make_unique<MovableComponent>();
	}
	else if (name == "renderable")
	{
		return createRenderable(attributes);
	}

	throw runtime_error("Component type not found: " + name);
}


hiage::ComponentManager::ComponentManager(Game& game) : game(game)
{
}

hiage::ComponentManager::~ComponentManager()
{
}

std::unique_ptr<Component> hiage::ComponentManager::createRenderable(const std::map<std::string, std::string>& attributes)
{
	const auto& spriteName = attributes.at(std::string("sprite"));

	SpriteManager::Resource* sprite = game.getSpriteManager().requestResourceCopy(spriteName);
	TextureManager::Resource* texture = game.getTextureManager().requestResourcePtr(sprite->strData1.c_str());

	if (!texture)
	{
		throw Exception(string("ERROR: Could not retrieve texture for sprite ") + spriteName);
	}
	sprite->resource->create(texture->resource, sprite->intData1, sprite->intData2);

	return make_unique<RenderableComponent>(*sprite->resource);
}


shared_ptr<Component> ComponentManager::createComponent(const std::string& type, const std::map<std::string, std::string>& attributes, const std::map<std::string, void*>& runtimeAttributes)
{
	if (componentCache.find(type) == componentCache.end())
		componentCache[type] = vector<shared_ptr<Component>>();

	componentCache[type].push_back(createComponentCore(type, attributes, runtimeAttributes));
	
	return componentCache[type][componentCache[type].size()-1];
}

std::vector<shared_ptr<Component>> hiage::ComponentManager::getComponentsOfType(const std::string& type)
{
	if (componentCache.find(type) == componentCache.end())
		return vector<shared_ptr<Component>>();

	return componentCache[type];
}
