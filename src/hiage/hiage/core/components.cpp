#include "components.hpp"
#include "game.h"
#include <memory>

using namespace hiage;
using namespace std;


/*
	ComponentManager
*/

hiage::ComponentManager::ComponentManager(Game& game) : game(game)
{
}

hiage::ComponentManager::~ComponentManager()
{
}

std::unique_ptr<Component> hiage::ComponentManager::createRenderable(const std::unordered_map<std::string, std::variant<std::string, double>>& properties) const
{
	const auto& spriteName = get<std::string>(properties.at(std::string("sprite")));

	auto& sprite = game.getSpriteManager().requestResourceCopy(spriteName);
	auto& texture = game.getTextureManager().requestResourcePtr(sprite->strData1.c_str());

	if (!texture)
	{
		throw Exception(string("ERROR: Could not retrieve texture for sprite ") + spriteName);
	}
	sprite->resource->create(texture->resource, sprite->intData1, sprite->intData2);

	return make_unique<RenderableComponent>(*sprite->resource);
}


unique_ptr<Component> ComponentManager::createComponent(const std::string& type, const std::unordered_map<std::string, std::variant<std::string, double>>& properties, const std::map<std::string, void*>& runtimeAttributes) const
{
	if (type == "physical")
	{
		double x = 0, y = 0;

		if (runtimeAttributes.find("x") != runtimeAttributes.end())
			x = *(double*)(runtimeAttributes.at("x"));

		if (runtimeAttributes.find("y") != runtimeAttributes.end())
			y = *(double*)(runtimeAttributes.at("y"));

		return make_unique<PositionComponent>(Vector2<double>(x, y));
	}
	else if (type == "movable")
		return make_unique<VelocityComponent>();
	else if (type == "renderable")
		return createRenderable(properties);
	else if (type == "physics")
		return make_unique<PhysicsComponent>();
	else if (type == "humancontroller")
		return make_unique<HumanControllerComponent>();
	else if (type == "collidable")
		return make_unique<CollidableComponent>();
	else if (type == "state")
		return make_unique<StateComponent>();
	else if (type == "boundingbox")
	{
		int x = 0, y = 0, width = 16, height = 16;

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

	throw runtime_error("Component type not found: " + type);
}

