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

EntityManager::EntityManager(const Game& game, const GameState& gameState) : _game(game), _gameState(gameState), _cacheVersion(0)
{

}

EntityManager::~EntityManager()
{
}

const std::unique_ptr<Entity>& EntityManager::createEntity(std::string objectName, const std::unordered_map<std::string, ComponentProperties>& componentRuntimeProperties)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;
	auto& objectManager = _game.getObjectManager();
	auto& objectDescResource = objectManager.requestResourcePtr(objectName);

	vector<shared_ptr<Component>> componentList;
	auto& componentFactory = _gameState.getComponentManager();

	for (auto& c : objectDescResource->resource->components)
	{
		ComponentProperties runtimeProperties;
		if (componentRuntimeProperties.contains(c.type))
			runtimeProperties = componentRuntimeProperties.at(c.type);
		
		shared_ptr<Component> cShared = componentFactory.createComponent(c, runtimeProperties);
		componentList.push_back(cShared);
	}

	auto ent = make_unique<Entity>(objectName);
	_components[ent->getEntityId()] = componentList;

	_entities.push_back(std::move(ent));
	_cacheVersion++;

	return _entities.back();
}

void hiage::EntityManager::addComponentToEntity(int entityId, const std::string& componentType, ComponentProperties& componentProperties)
{
	if (_components.contains(entityId))
	{
		auto& componentManager = _gameState.getComponentManager();

		shared_ptr<Component> cShared = componentManager.createComponent(componentType, componentProperties);
		_components[entityId].push_back(cShared);
		
		_cacheVersion++;
	}
	else
	{
		throw runtime_error(string("Entity with ID ") + entityId + " does not exist.");
	}
}

void hiage::EntityManager::removeComponentFromEntity(int entityId, const std::string& componentType)
{
	if (_components.contains(entityId))
	{
		auto& componentManager = _gameState.getComponentManager();
		int typeId = componentManager.getTypeIdForComponentType(componentType);

		auto& componentList = _components.at(entityId);

		for (size_t i = 0; i < componentList.size(); i++)
		{
			if (componentList[i]->getTypeId() == typeId)
			{
				componentList.erase(componentList.begin() + i);
				_cacheVersion++;
				break;
			}
		}
	}
	else
	{
		throw runtime_error(string("Entity with ID ") + entityId + " does not exist.");
	}
}

struct PositionComponentComparator {
private:
	EntityManager& _em;
	unordered_map<int, tuple<int, shared_ptr<CollidableComponent>, shared_ptr<PositionComponent>>> componentCache;
public:
	PositionComponentComparator(EntityManager& em) : _em(em)
	{
		auto components = _em.queryComponentGroup<CollidableComponent, PositionComponent>();
		for (auto& c : components)
		{
			componentCache[std::get<0>(c)] = c;
		}
	}

	bool operator()(const unique_ptr<Entity>& a,
		const unique_ptr<Entity>& b) const
	{
		auto id1 = a->getEntityId(), id2 = b->getEntityId();
		if (!componentCache.contains(id1) && !componentCache.contains(id2))
			return false;
		if (!componentCache.contains(id1))
			return true;
		if (!componentCache.contains(id2))
			return false;

		auto& o1 = componentCache.at(a->getEntityId());
		auto& o2 = componentCache.at(b->getEntityId());

		auto& bb1 = get<1>(o1);
		auto& bb2 = get<1>(o2);
		
		auto& pos1 = get<2>(o1);
		auto& pos2 = get<2>(o2);

		return pos1->getData().getX() + bb1->getData().boundingPolygon.getLeft() < pos2->getData().getX() + bb2->getData().boundingPolygon.getLeft();
	}
};


void hiage::EntityManager::sortEntitiesByPosition()
{
	PositionComponentComparator comp(*this);
	int  didSwap = 0;

	size_t i = 1;
	// Insertion sort - because we will almost always have a near-sorted array, and then the sorting time is near linear.
	while (i < _entities.size())
	{
		size_t j = i;
		while (j > 0 && comp(_entities[j], _entities[j - 1]))
		{
			std::swap(_entities[j], _entities[j-1]);
			didSwap++;
			j--;
		}
		i++;
	}

	if (didSwap > 0)
		_cacheVersion++;
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
