/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#include "entitymanager.hpp"
#include "game.hpp"
#include "gamestate.hpp"
#include "entity.hpp"
#include <tinyxml.h>

using namespace hiage;
using namespace std;

EntityManager::EntityManager(const Game& game, const GameState& gameState) : _game(game), _gameState(gameState)
{

}

EntityManager::~EntityManager()
{
}

void EntityManager::createEntity(std::string objectName, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;
	auto& objectManager = _game.getObjectManager();
	auto& objectDescResource = objectManager.requestResourcePtr(objectName);

	vector<shared_ptr<Component>> componentList;
	auto& componentFactory = _gameState.getComponentManager();

	for (auto& c : objectDescResource->resource->components)
	{
		shared_ptr<Component> cShared = componentFactory.createComponent(c, runtimeProperties);
		componentList.push_back(cShared);
	}

	auto ent = make_unique<Entity>(objectName);
	_components[ent->getEntityId()] = componentList;

	_entities.push_back(std::move(ent));
}

void EntityManager::createEntity(std::string objectName, const std::unordered_map<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>& componentRuntimeProperties)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;
	auto& objectManager = _game.getObjectManager();
	auto& objectDescResource = objectManager.requestResourcePtr(objectName);

	vector<shared_ptr<Component>> componentList;
	auto& componentFactory = _gameState.getComponentManager();

	for (auto& c : objectDescResource->resource->components)
	{
		std::unordered_map<std::string, std::variant<std::string, double>> runtimeProperties;
		if (componentRuntimeProperties.contains(c.type))
			runtimeProperties = componentRuntimeProperties.at(c.type);
		
		shared_ptr<Component> cShared = componentFactory.createComponent(c, runtimeProperties);
		componentList.push_back(cShared);
	}

	auto ent = make_unique<Entity>(objectName);
	_components[ent->getEntityId()] = componentList;

	_entities.push_back(std::move(ent));
}
const std::vector<std::unique_ptr<Entity>>& hiage::EntityManager::getEntities()
{
	return _entities;
}

void hiage::EntityManager::destroyAll()
{
	_entities.clear();
	_components.clear();
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

Font * FontFactory::createFont(const string& fontName, Game * game)
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
