#include "componentmanager.hpp"
#include "../gfx/sprite.h"
#include "game.hpp"

#include <exception>

using namespace hiage;

/*
	ComponentManager
*/

hiage::ComponentManager::ComponentManager(Game& game) : game(game)
{
	addGenericComponentFactory_2<PositionComponent>("physical");
	addGenericComponentFactory<VelocityComponent>("movable");
	addGenericComponentFactory<PhysicsComponent>("physics");
	addGenericComponentFactory<ControllerComponent>("controller");
	addGenericComponentFactory<ControllerStateComponent>("controllerstate");
	addGenericComponentFactory<CollidableComponent>("collidable");
	addComponentFactory<RenderableComponentFactory, RenderableComponent, const Game&>("renderable", game);
	addGenericComponentFactory<StateComponent>("state");
	addGenericComponentFactory<TrackingComponent>("objecttracker");
	addGenericComponentFactory<TrackableComponent>("trackable");
	addGenericComponentFactory<CameraComponent>("camera");
	addGenericComponentFactory<SpeedLimitComponent>("speedlimit");
}

hiage::ComponentManager::~ComponentManager()
{
}

const flecs::entity& ComponentManager::createComponent(flecs::entity& entity, const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const
 {
	auto& type = componentDescriptor.type;

	ComponentProperties mergedProperties;
	mergedProperties.insert(runtimeProperties.begin(), runtimeProperties.end());
	mergedProperties.insert(componentDescriptor.properties.begin(), componentDescriptor.properties.end());

	return createComponent(entity, type, mergedProperties);
}


const flecs::entity& ComponentManager::createComponent(flecs::entity& entity, const std::string& type, const ComponentProperties& properties) const
{
	if (_componentFactories.contains(type))
	{
		auto& factory = _componentFactories.at(type);
		return factory->createComponent(entity,  properties);
	}

	throw std::runtime_error("Component type not found: " + type);
}

/* 
    RenderableComponentFactory
*/

hiage::RenderableComponentFactory::RenderableComponentFactory(const Game& game) : game(game)
{
}

const flecs::entity& hiage::RenderableComponentFactory::createComponent(flecs::entity& entity, const ComponentProperties& properties) const
{
	const auto& spriteName = get<std::string>(properties.at(std::string("sprite")));

	auto sprite = game.getSpriteManager().requestResourceCopy(spriteName);
	sprite.resource.create(sprite.strData1, sprite.intData1, sprite.intData2);

	return entity.set<RenderableComponent>({ sprite.resource });
}