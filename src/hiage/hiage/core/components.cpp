#include "components.hpp"
#include "game.h"
#include <memory>

using namespace hiage;
using namespace std;


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

		return make_unique<PositionComponent>(Vector2<double>(x, y));
	}
	else if (name == "movable")
		return make_unique<VelocityComponent>();
	else if (name == "renderable")
		return createRenderable(attributes);
	else if (name == "physics")
		return make_unique<PhysicsComponent>();
	else if (name == "humancontroller")
		return make_unique<HumanControllerComponent>();
	else if (name == "collidable")
		return make_unique<CollidableComponent>();
	else if (name == "boundingbox")
	{
		int x = 0, y = 0, width = 16, height = 16;

		if (attributes.find("left") != attributes.end())
			x = std::stoi(attributes.at("left"));

		if (attributes.find("bottom") != attributes.end())
			y = std::stoi(attributes.at("bottom"));

		if (attributes.find("width") != attributes.end())
			width = std::stoi(attributes.at("width"));

		if (attributes.find("height") != attributes.end())
			height = std::stoi(attributes.at("height"));
		
		BoundingPolygon poly;
		poly.addVertex((double)x, y);
		poly.addVertex((double)x, (double)y + height);
		poly.addVertex((double)x + width, (double)y + height);
		poly.addVertex((double)x + width, y);
		poly.buildNormals();

		return make_unique<BoundingBoxComponent>(poly);
		
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
