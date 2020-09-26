/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#include "entitymanager.hpp"
#include "game.h"
#include "gamestate.hpp"
#include "entity.h"

using namespace hiage;
using namespace std;

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{
}

Entity& EntityManager::createObject(std::string objectName, Game * game, const GameState& gameState)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;

	std::string objectFile = game->getObjectFile(objectName);
	std::string objName = objectName;

	clog << "Loading information from file " << objectFile << endl << flush;
	if (!objectFile.length())
	{
		throw Exception("ERROR: No filename specified for object creation.");
	}

	SpriteManager::Resource * sprite = game->getSpriteManager().requestResourceCopy(game->getObjectSprite(objectName).c_str());
	if (!sprite)
	{
		throw Exception(string("ERROR: Could not retrieve sprite for object ") + objectName);

	}

	TextureManager::Resource * texture = game->getTextureManager().requestResourcePtr(sprite->strData1.c_str());
	if (!texture)
	{
		throw Exception(string("ERROR: Could not retrieve texture for object ") + objectName);
	}
	sprite->resource->create(texture->resource, sprite->intData1, sprite->intData2);

	PhysicalEntity * entity = new PhysicalEntity;
	entity->createFromFile(objectFile, sprite->resource, gameState);

	//Entity* ent = (Entity*)entity;
	entities.push_back(make_unique<PhysicalEntity>(*entity));

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
	FontManager::Resource * fontResource = game->getFontManager().requestResourcePtr(fontName.c_str());
	if (!fontResource)
	{
	    throw Exception("ERROR: Could not retrieve font.");
	}

	TextureManager::Resource * textureResource = game->getTextureManager().requestResourcePtr(fontResource->strData1.c_str());
	if (!textureResource)
	{
	    throw Exception("ERROR: Could not retrieve texture for font.");
	}

	font->create(textureResource->resource, fontResource->intData1, fontResource->intData2);
	font->setCharacterTable(fontResource->resource->getCharacterTable(), fontResource->resource->getTableCols(), fontResource->resource->getTableRows());

	return font;
}
