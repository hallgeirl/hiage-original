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
	addComponentFactory<GenericComponentFactory<PositionComponent>>("physical");
	addComponentFactory<GenericComponentFactory<VelocityComponent>>("movable");
	addComponentFactory<GenericComponentFactory<PhysicsComponent>>("physics");
	addComponentFactory<GenericComponentFactory<HumanControllerComponent>>("humancontroller");
	addComponentFactory<GenericComponentFactory<ControllerStateComponent>>("controllerstate");
	addComponentFactory<GenericComponentFactory<CollidableComponent>>("collidable");
	addComponentFactory<RenderableComponentFactory, const Game&>("renderable", game);
	addComponentFactory<GenericComponentFactory<StateComponent>>("state");
	addComponentFactory<GenericComponentFactory<TrackingComponent>>("objecttracker");
	addComponentFactory<GenericComponentFactory<TrackableComponent>>("trackable");
	addComponentFactory<GenericComponentFactory<CameraComponent>>("camera");
	addComponentFactory<GenericComponentFactory<SpeedLimitComponent>>("speedlimit");
}

hiage::ComponentManager::~ComponentManager()
{
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


BoundingPolygon hiage::CollidableComponent::createState(const ComponentProperties& properties)
{
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

	return poly;
}

Vector2<double> hiage::PositionComponent::createState(const ComponentProperties& properties)
{
	double x = 0, y = 0;

	if (properties.find("x") != properties.end())
		x = get<double>(properties.at("x"));

	if (properties.find("y") != properties.end())
		y = get<double>(properties.at("y"));

	return Vector2<double>(x, y);
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

State hiage::StateComponent::createState(const ComponentProperties& properties)
{
	State state;

	if (properties.contains("initial"))
		state.stateName = get<std::string>(properties.at("initial"));

	return state;
}


PhysicsProperties hiage::PhysicsComponent::createState(const ComponentProperties& properties)
{
	PhysicsProperties props;
	props.airResistance = props.groundFriction = props.hasGravity = 0;

	if (properties.contains("friction.air"))
		props.airResistance = get<double>(properties.at("friction.air"));

	if (properties.contains("friction.ground"))
		props.groundFriction = get<double>(properties.at("friction.ground"));

	if (properties.contains("gravity"))
		props.hasGravity = get<double>(properties.at("gravity")) != 0;

	return props;
}

SpeedLimitState hiage::SpeedLimitComponent::createState(const ComponentProperties& properties)
{
	SpeedLimitState state;
	state.speedLimit = Vector2<double>(-1, -1);

	if (properties.contains("speedLimit.x"))
		state.speedLimit.setX(get<double>(properties.at("speedLimit.x")));
	if (properties.contains("speedLimit.y"))
		state.speedLimit.setY(get<double>(properties.at("speedLimit.y")));

	return state;
}
