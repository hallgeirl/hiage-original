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

Entity& EntityManager::createEntity(std::string objectName, const std::map<std::string, void*>& attributes)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;

	std::string objectFile = game.getObjectFile(objectName);
	std::string objName = objectName;

	clog << "Loading information from file " << objectFile << endl << flush;
	if (!objectFile.length())
	{
		throw Exception("ERROR: No filename specified for object creation.");
	}

	clog << "Loading entity from file..." << endl;

	//load the xml file
	TiXmlDocument xmlDoc(objectFile.c_str());
	if (!xmlDoc.LoadFile())
	{
		throw IOException(string("ERROR: Could not open XML file ") + objectFile);
	}

	TiXmlElement* objectElement = 0;
	objectElement = xmlDoc.FirstChildElement("object");

	//check if it's a texture file
	if (!objectElement)
	{
		throw IOException(string("ERROR: Could not find XML element <object> "));
	}

	//type = objectElement->Attribute("type");
	auto entityName = objectElement->Attribute("name");
	auto entityType = objectElement->Attribute("type");
	vector<shared_ptr<Component>> componentList;

	//store the sprite name in StrData1
	//spriteElement = objectElement->FirstChildElement("sprite");

	TiXmlElement* componentsElement = objectElement->FirstChildElement("components");
	if (componentsElement)
	{
		TiXmlElement* componentElement = componentsElement->FirstChildElement("component");
		string componentType;

		auto componentFactory = gameState.getComponentManager();
		while (componentElement)
		{
			componentType = componentElement->Attribute("type");
			map<string, string> componentAttributes;

			auto childElement = componentElement->FirstChildElement();
			while (childElement)
			{
				std::string key = childElement->Value();
				std::string value = childElement->GetText();
				componentAttributes[key] = value;

				childElement = childElement->NextSiblingElement();
			}

			componentList.push_back(componentFactory.createComponent(componentType, componentAttributes, attributes));
			componentElement = componentElement->NextSiblingElement("component");
		}
	}

	//store the script functions to use
	/*
	* This must be rewritten for ECS
	*
	TiXmlElement* scriptsElement = objectElement->FirstChildElement("scripts");

	if (scriptsElement)
	{
		TiXmlElement* scriptElement = scriptsElement->FirstChildElement("script");
		string scriptType, scriptFunction;

		while (scriptElement)
		{
			scriptType = scriptElement->Attribute("type");
			scriptFunction = scriptElement->Attribute("function");

			if (!scriptType.length() || !scriptFunction.length())
			{
				clog << "Warning: Malformed object XML file in <script>: Missing type or function attribute (or they are at 0 length)." << endl;
				continue;
			}

			if (!scriptType.compare("init"))
			{
				initScripts.push_back(scriptFunction);
			}
			else if (!scriptType.compare("update"))
			{
				updateScripts.push_back(scriptFunction);
			}
			else if (!scriptType.compare("collision"))
			{
				collisionScripts.push_back(scriptFunction);
			}
			else if (!scriptType.compare("objcollision"))
			{
				objectCollisionScripts.push_back(scriptFunction);
			}

			scriptElement = scriptElement->NextSiblingElement("script");
		}
	}
	*/

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
