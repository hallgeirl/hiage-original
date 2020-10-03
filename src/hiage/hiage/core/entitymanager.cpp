/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#include "entitymanager.hpp"
#include "game.h"
#include "gamestate.hpp"
#include "entity.h"
#include <tinyxml.h>

using namespace hiage;
using namespace std;

EntityManager::EntityManager(const Game& game, const GameState& gameState) : game(game), gameState(gameState)
{

}

EntityManager::~EntityManager()
{
}

Entity& EntityManager::createEntity(std::string objectName, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;
	auto& objectManager = game.getObjectManager();
	auto& objectDescResource = objectManager.requestResourcePtr(objectName);

	//type = objectElement->Attribute("type");
	const auto& entityName = objectDescResource->resource->name;
	const auto& entityType = objectDescResource->resource->type;
	
	vector<shared_ptr<Component>> componentList;
	auto& componentFactory = gameState.getComponentManager();

	for (auto& c : objectDescResource->resource->components)
	{
		auto& componentType = c.type;
		shared_ptr<Component> cShared = componentFactory.createComponent(componentType, c.properties, runtimeProperties);
		componentList.push_back(cShared);
	}

	//Entity* ent = (Entity*)entity;
	auto ent = make_unique<Entity>();
	components[ent->getEntityId()] = componentList;

	entities.push_back(std::move(ent));

	return *entities[entities.size() - 1];
}

int EntityManager::getObjectCount()
{
	return entities.size();
}

/*
    Font factory class
*/

FontFactory::FontFactory()
{
}

FontFactory::~FontFactory()
{
}

Font * FontFactory::createFont(string fontName, Game * game)
{
    Font * font = new Font();
    //attempt to load the main font
	auto& fontResource = game->getFontManager().requestResourcePtr(fontName.c_str());
	if (!fontResource)
	{
	    throw Exception("ERROR: Could not retrieve font.");
	}

	auto& textureResource = game->getTextureManager().requestResourcePtr(fontResource->strData1.c_str());
	if (!textureResource)
	{
	    throw Exception("ERROR: Could not retrieve texture for font.");
	}

	font->create(textureResource->resource, fontResource->intData1, fontResource->intData2);
	font->setCharacterTable(fontResource->resource->getCharacterTable(), fontResource->resource->getTableCols(), fontResource->resource->getTableRows());

	return font;
}
