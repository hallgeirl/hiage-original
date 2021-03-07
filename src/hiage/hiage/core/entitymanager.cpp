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

EntityManager::EntityManager(const hiage::Game& game, const GameState& gameState, flecs::world& ecs) : _game(game), _gameState(gameState), _cacheVersion(0), _ecs(ecs)
{

}

EntityManager::~EntityManager()
{
}

flecs::entity& EntityManager::createEntity(std::string objectName, const std::unordered_map<std::string, ComponentProperties>& componentRuntimeProperties)
{
	std::clog << "Creating object " << objectName << "..." << std::endl;
	auto& objectManager = _game.getObjectManager();
	auto& objectDescResource = objectManager.requestResourceRef(objectName);

	auto& componentFactory = _gameState.getComponentManager();
	auto e = _ecs.entity();

	for (const auto& c : objectDescResource.resource.components)
	{
		ComponentProperties runtimeProperties;
		if (componentRuntimeProperties.contains(c.type))
			runtimeProperties = componentRuntimeProperties.at(c.type);
		
		componentFactory.createComponent(e, c, runtimeProperties);
	}
	
	_entities.push_back(e);
	_cacheVersion++;

	return _entities.back();
}

void hiage::EntityManager::addComponentToEntity(flecs::entity& entity, const std::string& componentType, ComponentProperties& componentProperties)
{
	auto& componentManager = _gameState.getComponentManager();
		
	componentManager.createComponent(entity, componentType, componentProperties);
}

void hiage::EntityManager::removeComponentFromEntity(int entityId, const std::string& componentType)
{
/*	TODO flecs
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
	}*/
}
/*
* TODO flecs
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

*/

void hiage::EntityManager::destroyAll()
{
	_entities.clear();
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
	auto& fontResource = game->getFontManager().requestResourceRef(fontName.c_str());

	auto& textureResource = game->getTextureManager().requestResourceRef(fontResource.strData1.c_str());

	font->create(textureResource.resource, fontResource.intData1, fontResource.intData2);
	font->setCharacterTable(fontResource.resource.getCharacterTable(), fontResource.resource.getTableCols(), fontResource.resource.getTableRows());

	return font;
}
