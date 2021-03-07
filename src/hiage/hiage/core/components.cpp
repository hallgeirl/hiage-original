#include "components.hpp"
#include "game.hpp"
#include <memory>

using namespace hiage;
using namespace std;


CollidableProperties hiage::CollidableComponent::createState(const ComponentProperties& properties)
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

	CollidableProperties props;
	props.boundingPolygon = poly;

	return props;
}

hiage::PositionComponent::PositionComponent(const ComponentProperties& properties) : pos(0, 0)
{

	if (properties.find("x") != properties.end())
		pos.x = get<double>(properties.at("x"));

	if (properties.find("y") != properties.end())
		pos.y = get<double>(properties.at("y"));
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

TrackingComponentProperties hiage::TrackingComponent::createState(const ComponentProperties& properties)
{
	TrackingComponentProperties props;
	props.mode = "fixed";
	if (properties.contains("mode"))
		props.mode = get<string>(properties.at("mode"));

	return props;
}

ControllerProperties hiage::ControllerComponent::createState(const ComponentProperties& properties)
{
	ControllerProperties props;
	props.controllerType = "none";
	if (properties.contains("controllerType"))
		props.controllerType = get<string>(properties.at("controllerType"));
	
	if (props.controllerType == "constant" && properties.contains("constantActions"))
	{
		props.constantActions = std::get<std::vector<std::string>>(properties.at("constantActions"));
	}

	return props;
}

CameraProperties hiage::CameraComponent::createState(const ComponentProperties& properties)
{
	CameraProperties props;
	props.zoom = 400;

	if (properties.contains("zoom"))
		props.zoom = get<double>(properties.at("zoom"));

	return props;
}
