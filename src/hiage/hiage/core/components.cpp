#include "components.hpp"
#include "game.hpp"
#include <memory>

using namespace hiage;
using namespace std;


hiage::CollidableComponent::CollidableComponent(const ComponentProperties& properties)
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

	boundingPolygon = poly;
}

hiage::PositionComponent::PositionComponent(const ComponentProperties& properties) : pos(0, 0)
{

	if (properties.find("x") != properties.end())
		pos.x = get<double>(properties.at("x"));

	if (properties.find("y") != properties.end())
		pos.y = get<double>(properties.at("y"));
}

hiage::VelocityComponent::VelocityComponent(const ComponentProperties& properties) : vel(0, 0)
{

	if (properties.find("x") != properties.end())
		vel.x = get<double>(properties.at("x"));

	if (properties.find("y") != properties.end())
		vel.y = get<double>(properties.at("y"));
}


hiage::StateComponent::StateComponent(const ComponentProperties& properties)
{
	if (properties.contains("initial"))
		stateName = get<std::string>(properties.at("initial"));
}


hiage::PhysicsComponent::PhysicsComponent(const ComponentProperties& properties)
{
	if (properties.contains("friction.air"))
		airResistance = get<double>(properties.at("friction.air"));

	if (properties.contains("friction.ground"))
		groundFriction = get<double>(properties.at("friction.ground"));

	if (properties.contains("gravity"))
		hasGravity = get<double>(properties.at("gravity")) != 0;
}

hiage::SpeedLimitComponent::SpeedLimitComponent(const ComponentProperties& properties)
{
	speedLimit = Vector2<double>(-1, -1);

	if (properties.contains("speedLimit.x"))
		speedLimit.setX(get<double>(properties.at("speedLimit.x")));
	if (properties.contains("speedLimit.y"))
		speedLimit.setY(get<double>(properties.at("speedLimit.y")));
}

hiage::TrackingComponent::TrackingComponent(const ComponentProperties& properties)
{
	mode = "fixed";
	if (properties.contains("mode"))
		mode = get<string>(properties.at("mode"));
}

hiage::ControllerComponent::ControllerComponent(const ComponentProperties& properties)
{
	controllerType = "none";
	if (properties.contains("controllerType"))
		controllerType = get<string>(properties.at("controllerType"));
	
	if (controllerType == "constant" && properties.contains("constantActions"))
	{
		constantActions = std::get<std::vector<std::string>>(properties.at("constantActions"));
	}
}

hiage::CameraComponent::CameraComponent(const ComponentProperties& properties)
{
	if (properties.contains("zoom"))
		zoom = get<double>(properties.at("zoom"));
}
