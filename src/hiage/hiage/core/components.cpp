#include "components.hpp"
#include "game.hpp"
#include <memory>

using namespace hiage;
using namespace std;


/*
	ComponentManager
*/

hiage::ComponentManager::ComponentManager(Game& game) : game(game)
{
	addComponentFactory<PhysicalComponentFactory>("physical");
	addComponentFactory<GenericComponentFactory<VelocityComponent>>("movable");
	addComponentFactory<PhysicsComponentFactory>("physics");
	addComponentFactory<GenericComponentFactory<HumanControllerComponent>>("humancontroller");
	addComponentFactory<GenericComponentFactory<CollidableComponent>>("collidable");
	addComponentFactory<BoundingBoxComponentFactory>("boundingbox");
	addComponentFactory<RenderableComponentFactory, const Game&>("renderable", game);
	addComponentFactory<StateComponentFactory>("state");
	addComponentFactory<GenericComponentFactory<TrackingComponent>>("objecttracker");
	addComponentFactory<GenericComponentFactory<TrackableComponent>>("trackable");
	addComponentFactory<GenericComponentFactory<CameraComponent>>("camera");
}

hiage::ComponentManager::~ComponentManager()
{
}

std::unique_ptr<Component> hiage::ComponentManager::createRenderable(const ComponentProperties& properties) const
{
	const auto& spriteName = get<std::string>(properties.at(std::string("sprite")));

	auto sprite = game.getSpriteManager().requestResourceCopy(spriteName);
	auto& texture = game.getTextureManager().requestResourcePtr(sprite->strData1.c_str());

	if (!texture)
	{
		throw Exception(string("ERROR: Could not retrieve texture for sprite ") + spriteName);
	}
	sprite->resource->create(texture->resource, sprite->intData1, sprite->intData2);

	return make_unique<RenderableComponent>(*sprite->resource);
}


unique_ptr<Component> ComponentManager::createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const
{
	auto& type = componentDescriptor.type;

	if (componentFactories.contains(type))
	{
		auto& factory = componentFactories.at(componentDescriptor.type);
		return factory->createComponent(componentDescriptor, runtimeProperties);
	}

	throw runtime_error("Component type not found: " + type);
}

std::unique_ptr<Component> hiage::BoundingBoxComponentFactory::createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties&) const
{
	auto& properties = componentDescriptor.properties;

	double x = 0, y = 0, width = 16, height = 16;

	if (properties.find("left") != properties.end())
		x = get<double>(properties.at("left"));

	if (properties.find("bottom") != properties.end())
		y = get<double>(properties.at("bottom"));

	if (properties.find("width") != properties.end())
		width = get<double>(properties.at("width"));

	if (properties.find("height") != properties.end())
		height = get<double>(properties.at("height"));

	BoundingPolygon poly;
	poly.addVertex((double)x, y);
	poly.addVertex((double)x, (double)y + height);
	poly.addVertex((double)x + width, (double)y + height);
	poly.addVertex((double)x + width, y);
	poly.buildNormals();

	return make_unique<BoundingBoxComponent>(poly);
}

std::unique_ptr<Component> hiage::PhysicalComponentFactory::createComponent(const ComponentDescriptor&, const ComponentProperties& runtimeProperties) const
{
	double x = 0, y = 0;

	if (runtimeProperties.find("x") != runtimeProperties.end())
		x = get<double>(runtimeProperties.at("x"));

	if (runtimeProperties.find("y") != runtimeProperties.end())
		y = get<double>(runtimeProperties.at("y"));

	return make_unique<PositionComponent>(Vector2<double>(x, y));
}

hiage::RenderableComponentFactory::RenderableComponentFactory(const Game& game) : game(game)
{
}

std::unique_ptr<Component> hiage::RenderableComponentFactory::createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties&) const
{
	auto& properties = componentDescriptor.properties;

	const auto& spriteName = get<std::string>(properties.at(std::string("sprite")));

	auto sprite = game.getSpriteManager().requestResourceCopy(spriteName);
	auto& texture = game.getTextureManager().requestResourcePtr(sprite->strData1.c_str());

	if (!texture)
	{
		throw Exception(string("ERROR: Could not retrieve texture for sprite ") + spriteName);
	}
	sprite->resource->create(texture->resource, sprite->intData1, sprite->intData2);

	return make_unique<RenderableComponent>(*sprite->resource);
}

std::unique_ptr<Component> hiage::StateComponentFactory::createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties&) const
{
	auto& properties = componentDescriptor.properties;
	auto& initialState = get<std::string>(properties.at("initial"));
	
	State state;
	state.stateName = initialState;
	return make_unique<StateComponent>(state);
}

std::unique_ptr<Component> hiage::PhysicsComponentFactory::createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties&) const
{
	auto& properties = componentDescriptor.properties;

	PhysicsProperties props;

	if (properties.contains("friction.air"))
		props.airResistance = get<double>(properties.at("friction.air"));

	if (properties.contains("friction.ground"))
		props.groundFriction = get<double>(properties.at("friction.ground"));

	if (properties.contains("gravity"))
		props.hasGravity = get<double>(properties.at("gravity")) != 0;

	return make_unique<PhysicsComponent>(props);
}
