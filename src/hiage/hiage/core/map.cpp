/*!
	\file game_map.cpp

	\author Hallgeir Lien

	Description:
*/

#include "map.hpp"
#include "entitymanager.hpp"
#include "../lua-includes.h"

#include <base64/base64.hpp>
#include <json/json.hpp>

#include <iostream>
#include <fstream>

using namespace hiage;
using namespace std;

using json = nlohmann::json;
using Base64 = macaron::Base64;

Map::Map(Game &game, GameState& gameState) : _game(game), _gameState(gameState), _tilemap()
{
    _background = 0;
    _objectDeletedFlag = false;
    _updateOffscreen = false;

    //temp
    _mapToCreate = "";
    _createMapQueuedFlag = false;
}

Map::~Map()
{
    destroy();
}

const int Map::MAPVERSION = 100;


void Map::createFromFile(std::string path, bool runScripts)
{

#pragma warning (push)
#pragma warning (disable:26451) // arithmetic overflow warning on indexing
	//map loading goes here
	clog << "Loading map from file " << path << endl << flush;

	int width, height, layers, tilesize;
	long temp, version;
	char *buffer;

    //delete the old map
    destroy();

    ifstream file(_game.getResourcePath(path).c_str());
	if (!file.is_open())
	{
		throw IOException(string("ERROR: Could not open file ") + path);
	}

	file.read((char*)&version, 4);
	file.read((char*)&width, 4);
	file.read((char*)&height, 4);
	file.read((char*)&layers, 4);
	file.read((char*)&tilesize, 4);

	clog << "- Dimensions: " << width << "x" << height << "x" << layers << endl << flush;

	//read tileset name
	file.read((char*)&temp, 4);
	buffer = new char[temp+1];
	file.read(buffer, temp);
	buffer[temp] = 0;

	//create the map and set the tileset
	_tilemap.createMap(width,height,layers,tilesize);

	_tilemap.setTileset(_game.getTilesetManager().requestResourcePtr(buffer)->resource);
	_tilesetName = buffer;

	delete [] buffer;

	//read background name
	file.read((char*)&temp, 4);
	buffer = new char[temp+1];
	file.read(buffer, temp);
	buffer[temp] = 0;
    setBackground(buffer);
    _backgroundName = buffer;

	delete [] buffer;

    //scripts
    //script files to include

    //number of include scripts
    file.read((char*)&temp, 4);

    //script filenames
    for (int i = 0; i < temp; i++)
    {
        int temp2;
        //length of file name
        file.read((char*)&temp2, 4);
        buffer = new char[temp2+1];
        buffer[temp2] = 0;

        //file name of the script to include
        file.read(buffer, temp2);
        _includeScripts.push_back(string(buffer));
        delete [] buffer;
    }

    //number of init scripts
    file.read((char*)&temp, 4);

    //script functions
    for (int i = 0; i < temp; i++)
    {
        int temp2;
        //length of function name
        file.read((char*)&temp2, 4);
        buffer = new char[temp2+1];
        buffer[temp2] = 0;

        //function name
        file.read(buffer, temp2);
        _initScripts.push_back(string(buffer));

        delete [] buffer;
    }

    //number of update scripts
    file.read((char*)&temp, 4);

    //script functions
    for (int i = 0; i < temp; i++)
    {
        int temp2;
        //length of function name
        file.read((char*)&temp2, 4);
        buffer = new char[temp2+1];
        buffer[temp2] = 0;

        //function name
        file.read(buffer, temp2);
        _updateScripts.push_back(string(buffer));

        delete [] buffer;
    }

    //number of shutdown scripts
    file.read((char*)&temp, 4);

    //script functions
    for (int i = 0; i < temp; i++)
    {
        int temp2;
        //length of function name
        file.read((char*)&temp2, 4);
        buffer = new char[temp2+1];
        buffer[temp2] = 0;

        //function name
        file.read(buffer, temp2);
        _shutdownScripts.push_back(string(buffer));

        delete [] buffer;
    }



	//load tile data
	for (int z = 0; z < layers; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int tile;
				file.read((char*)&tile, 4);
				_tilemap.setTile(x,y,z,tile);
			}
		}
	}

	//load objects
	file.read((char*)&temp, 4);
	if (temp < 0)
	{
		throw IOException("ERROR: Number of objects is less than 0.");
	}


    auto& em = _gameState.getEntityManager();
    em.destroyAll();

	for (int i = 0; i < temp; i++)
	{
		int buffsize;
		double objx, objy;

		//get object name
		file.read((char*)&buffsize, 4);
		buffer = new char[buffsize+1];

		file.read(buffer, buffsize);
		buffer[buffsize] = 0;

		//position
		file.read((char*)&objx, 8);
		file.read((char*)&objy, 8);

		clog << "Creating object " << buffer << " at (" << objx << ", " << objy << ")\n" << flush;
        
        em.createEntity(buffer, {
            { "x", objx }, 
            { "y", objy }
        });

		delete [] buffer;
	}

	//include the script files
    for (unsigned int i = 0; i < _includeScripts.size(); i++)
    {
        _game.scriptVM.runFile(_includeScripts[i]);
    }

    if (runScripts)
    {
        //run initialization scripts
        for (unsigned int i = 0; i < _initScripts.size(); i++)
        {
            _game.scriptVM.executeLine(_initScripts[i] + "()");
        }
    }
#pragma warning(pop)
}

void hiage::Map::loadFromJson(std::string path, bool runScripts)
{
    string fullPath = _game.getResourcePath(path);
    clog << "Loading map from file " << path << endl << flush;

    destroy();

    std::ifstream t(fullPath);
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());

    auto j = json::parse(str);

    auto& version = j.at("version");

    clog << "Map version: " << version << endl;
    auto& dimensions = j.at("dimensions");

    int height = dimensions.at("height"),
        width = dimensions.at("width"),
        layers = dimensions.at("layers"),
        tileSize = dimensions.at("tileSize");


	clog << "- Dimensions: " << width << "x" << height << "x" << layers << endl << flush;

    _tilemap.createMap(width, height, layers, tileSize);

    // Visuals (tileset+background)
    auto& visual = j.at("visual");
    string tilesetName = visual.at("tilesetName");
    string backgroundName = visual.at("backgroundName");

    _tilemap.setTileset(_game.getTilesetManager().requestResourcePtr(tilesetName)->resource);
    setBackground(backgroundName);

    // Load scripts
    auto& scripts = j.at("scripts");

    for (string script : scripts.at("include"))
        _includeScripts.push_back(script);

    for (string script : scripts.at("init"))
        _initScripts.push_back(script);

    for (string script : scripts.at("update"))
        _updateScripts.push_back(script);

    for (string script : scripts.at("shutdown"))
        _updateScripts.push_back(script);

    // Load tile data
    string tileDatab64 = j.at("tileData");
    _tilemap.importMap(Base64::Decode<uint32_t>(tileDatab64));


    // Load objects
    auto& objects = j.at("entities");

    auto& em = _gameState.getEntityManager();

    for (auto& o : objects)
    {
        string objName = o.at("name");

        auto& components = o.at("components");
        
        for (auto& c : components)
        {
            auto type = c.at("type");
            
        }
        /*em.createEntity(buffer, {
            { "x", objx },
            { "y", objy }
            });*/
    }

    /*

	for (int i = 0; i < temp; i++)
	{
		int buffsize;
		double objx, objy;

		//get object name
		file.read((char*)&buffsize, 4);
		buffer = new char[buffsize+1];

		file.read(buffer, buffsize);
		buffer[buffsize] = 0;

		//position
		file.read((char*)&objx, 8);
		file.read((char*)&objy, 8);

		clog << "Creating object " << buffer << " at (" << objx << ", " << objy << ")\n" << flush;
        
        em.createEntity(buffer, {
            { "x", objx }, 
            { "y", objy }
        });

		delete [] buffer;
	}

	//include the script files
    for (unsigned int i = 0; i < includeScripts.size(); i++)
    {
        gameInstance.scriptVM.runFile(includeScripts[i]);
    }
     */

    if (runScripts)
    {
        //run initialization scripts
        for (unsigned int i = 0; i < _initScripts.size(); i++)
        {
            _game.scriptVM.executeLine(_initScripts[i] + "()");
        }
    }
    
   
}

void Map::queueCreateMap(string path)
{
    _mapToCreate = path;
    _createMapQueuedFlag = true;
}

void Map::createFromFile(string path)
{
    createFromFile(path, true);
}

//create an empty map (deletes any current content)
void Map::createEmpty(int width, int height, int layers, int tileSize, bool onlyTiles)
{
    //If onlyTiles is false, destroy everything (objects, scripts etc.). Otherwise, just create a new tilemap.
    if (!onlyTiles)
    {
        destroy();
    }
    _tilemap.createMap(width, height, layers, tileSize);
}

void Map::saveAsJson(string path)
{
    clog << "Saving map to file " << path.c_str() << endl << flush;
    
    int mapWidth = _tilemap.getWidth();
    int mapHeight = _tilemap.getHeight();
    int mapLayers = _tilemap.getLayers();
    int tileSize = _tilemap.getTileSize();

    //check for valid dimensions
    if (mapWidth <= 0 || mapHeight <= 0 || mapLayers <= 0)
    {
        throw Exception("ERROR: Invalid map dimensions.");
    }

    auto& em = _gameState.getEntityManager();

    vector<json> objectsToSave;
    for (auto& e : em.getEntities())
    {
        auto comp = em.queryComponentGroup<PositionComponent>(e->getEntityId());
        vector<json> componentsJson;

        if (comp != nullptr)
        {
            json compJson = {
                { "type", "physical" },
                { "runtimeProperties", {
                    { "x", comp->getData().getX() },
                    { "y", comp->getData().getY() },
                }}
            };

            componentsJson.push_back(compJson);
            
        }

        json obj = {
            { "name", e->getName() },
            { "components", componentsJson }
        };

        objectsToSave.push_back(obj);
    }

    json j = {
        { "version", MAPVERSION },
        { "dimensions", {
            { "width", mapWidth },
            { "height", mapHeight },
            { "layers", mapLayers },
            { "tileSize", tileSize }
        }},
        { "visual", {
            { "tilesetName", _tilesetName },
            { "backgroundName", _backgroundName }
        }},
        { "scripts", {
            { "include", _includeScripts },
            { "init", _initScripts },
            { "update", _updateScripts },
            { "shutdown", _shutdownScripts }
        }},
        { "tileData", Base64::Encode<uint32_t>(_tilemap.getTilemapRaw())},
        { "entities", objectsToSave }
    };

    auto jsonSource = j.dump(2);

    //open file
    ofstream file(path.c_str());
    if (!file.is_open())
    {
        throw IOException(string("ERROR: Could not load file ") + path);
    }

    file.write(jsonSource.c_str(), jsonSource.length());

    file.close();

    clog << "Map saving complete.\n" << flush;
}

//destroy the map content
void Map::destroy()
{
    _tilemap.destroy();

    auto& em = _gameState.getEntityManager();
    em.destroyAll();
    _includeScripts.clear();
    _initScripts.clear();
    _updateScripts.clear();
    _shutdownScripts.clear();

    //reset scripting
    _game.scriptVM.executeLine("map=nil");
    luabind::globals(_game.scriptVM.getVm())["map"] = this;
	_game.scriptVM.executeLine("map[\"objects\"]={}");
	_background = 0;
}


//create a new object on the map
/*PhysicalEntity &Map::createObject(std::string name, double x, double y, bool runScripts)
{
    //create the actual object
    auto& em = gameState.getEntityManager();
    em.createObject(name, &gameInstance, gameState);

	int o = em.getObjectCount() - 1;
	
    //set it's position
    auto& obj = em.getObjectByIndex<PhysicalEntity>(o);
	obj.setPosition(x, y);

    //make a reference to the object in the script (allowing us to modify non-hardcoded attributes and member functions INSIDE the script)
	//gameInstance.scriptVM.executeLine(string("map.objects[") + (o+1) + "]=map:getobject(" + (int)(o) + ")");
	//gameInstance.scriptVM.executeLine(string("map.objects[") + (o+1) + "].id=" + o);

    //execute init scripts (in reverse order to execute them in the order they were created)
    if (runScripts)
    {
        for (int i = obj.initScripts.size() - 1; i >= 0; i--)
        {
            //gameInstance.scriptVM.executeLine(obj.initScripts[i] + "(map.objects[" + (int)(o+1) + "])");
        }
    }

    //return a reference to the object (for use in the scripts for instance)
	return (obj);
}
*/
/*PhysicalEntity &Map::createObject(std::string name, double x, double y)
{
    return createObject(name, x, y, true);
}*/

/*

PhysicalEntity& Map::getObject(int index)
{
    auto& em = gameState.getEntityManager();
    auto& obj = em.getObjectByIndex<PhysicalEntity>(index);
    return obj;
}


PhysicalEntity& Map::getObjectAt(double x, double y)
{
    auto& em = gameState.getEntityManager();
    
    //find the object at position (x,y)
	for (size_t i = 0; i < em.getObjectCount(); i++)
	{
        auto& obj = em.getObjectByIndex<PhysicalEntity>(i);
		Rect colBox;// = obj.getSprite().getCollisionBox();
		colBox.left = obj.getX();
		colBox.bottom = obj.getY();
		colBox.right = obj.getX() + obj.getSprite()->getWidth();
		colBox.top = obj.getY() + obj.getSprite()->getHeight();
		if (x > colBox.left && x < colBox.right && y < colBox.top && y > colBox.bottom)
		{
			//objects.erase(objects.begin() + i);
			//i--;
			return obj;
		}
	}

	throw Exception(string("Warning: No object at position (") + x + "," + y + ")");
}



size_t Map::getObjectCount()
{
    return gameState.getEntityManager().getObjectCount();
}

//delete the object at the specified index
void Map::deleteObject(int index)
{
    auto& em = gameState.getEntityManager();
    auto& obj = em.getObjectByIndex<Entity>(index);
    obj.setDestructionFlag(true);
}
*/
/*
* 
* TODO - reimplement later
void Map::deleteObjectAt(double x, double y)
{
    //find the object at position (x,y)
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		Rect colBox;// = obj.getSprite().getCollisionBox();
		colBox.left = obj.getX();
		colBox.bottom = obj.getY();
		colBox.right = obj.getX() + obj.getSprite()->getWidth();
		colBox.top = obj.getY() + obj.getSprite()->getHeight();
		if (x > colBox.left && x < colBox.right && y < colBox.top && y > colBox.bottom)
		{
			objects.erase(objects.begin() + i);
			break;
		}
	}
}
*/


//Render the map
void Map::render()
{
    Display &disp = _game.getDisplay();
    Renderer &renderer = disp.getRenderer();
    double aspect = disp.getAspectRatio();
    double zoom = disp.getZoom();
    double camX = disp.getCamX();
    double camY = disp.getCamY();


    //Render the tilemap
    ObjectZ depth = ObjectZ::MIDDLE_BACK;
	for (int i = 0; i < _tilemap.getLayers(); i++)
	{
	    switch (i)
	    {
	        case 0:
                depth = ObjectZ::MIDDLE_BACK;
                break;

            case 1:
                depth = ObjectZ::NEAR_BACK;
                break;

            case 2:
                depth = ObjectZ::MIDDLE_FRONT;
                break;

            case 3:
                depth = ObjectZ::BACK;
                break;

            case 4:
                depth = ObjectZ::NEAR_FRONT;
                break;

            case 5:
                depth = ObjectZ::FURTHEST;
                break;

            case 6:
                depth = ObjectZ::FRONT;
                break;

            case 7:
                depth = ObjectZ::CLOSEST;
                break;

	    }
	    _tilemap.render(renderer, camX, camY, zoom, aspect, depth, i);
	}

	//and the background
	//TODO: add support for choosing parallax options
    if (_background)
    {
        double offset;
        offset = disp.getCamX() / 1.5;

        double startx = (disp.getCamX() - (float)disp.getViewWidth() - offset) / _background->getWidth();
        double endx = startx + ((float)disp.getViewWidth() / (float)_background->getWidth());
        double y = disp.getCamY() - (disp.getViewHeight() / 2);

        startx -= 1.0f;
        endx += 1.0f;

        renderer.beginRender(ObjectZ::BACK, _background);
        for (int x = (int)startx; x <= (int)endx; x++)
        {
            renderer.addVertex((double)x * _background->getWidth() + offset, (double)y + _background->getHeight(), 0, 0);
            renderer.addVertex((double)x * _background->getWidth() + _background->getWidth() + offset, (double)y + _background->getHeight(), 1, 0);
            renderer.addVertex((double)x * _background->getWidth() + _background->getWidth() + offset, y, 1, 1);
            renderer.addVertex((double)x * _background->getWidth() + offset, y, 0, 1);

        }

        renderer.endRender();
    }
}

void Map::update(double)
{
/*    Display & disp = gameInstance.getDisplay();

    double aspect = disp.getAspectRatio();
    double zoom = disp.getZoom();
    double camX = disp.getCamX();
    double camY = disp.getCamY();

    const double margin = 64;
    Vector2<double> colpos1, colpos2;

    double viewLeft = camX - (zoom * aspect) - margin;
    double viewRight = camX + (zoom * aspect) + margin;
    double viewTop = camY + zoom + margin;
    double viewBottom = camY - zoom - margin;

    auto& em = gameState.getEntityManager();

    for (size_t i = 0; i  < em.getObjectCount(); i++)
    {
        auto& obj = em.getObjectByIndex<PhysicalEntity>(i);
        //ignore objects that are set to be destroyed
        if (obj.getDestructionFlag())
        {
            continue;
        }

	    //check if the object is inside the viewport and if the updateOffscreen flag is set
	    if (!((obj.getX() + obj.getSprite()->getWidth() >= viewLeft) && (obj.getX() <= viewRight))
            || !((obj.getY() + obj.getSprite()->getHeight() >= viewBottom) && (obj.getY() <= viewTop))
            && !updateOffscreen)
        {
            continue;
        }


        //execute update scripts (in reverse order to execute them in the order they were created)
        for (int j = obj.updateScripts.size() - 1; j >= 0 && (em.getObjectCount() - 1) >= i; j--)
        {
            gameInstance.scriptVM.executeLine(obj.updateScripts[j] + "(map.objects[" + (int)(i+1) + "])");
        }

        if ((int)i > (int)(em.getObjectCount()-1))
        {
            return;
        }

        //update the objects
        obj.update(frameTime);

        //check for collisions
        for (size_t j = i+1; j < em.getObjectCount(); j++)
        {
            auto& obj = em.getObjectByIndex<PhysicalEntity>(j);
            //ignore objects that are set to be destroyed
            if (obj.getDestructionFlag())
            {
                continue;
            }
            //check the distance between them
            double distance = (obj.getPosition() - obj.getPosition()).length();
            if (distance > 50)
            {
                continue;
            }

            //TODO: Put in a better distance check.

            //check if the object is inside the viewport and if the updateOffscreen flag is set
            if (!((obj.getX() + obj.getSprite()->getWidth() >= viewLeft) && (obj.getX() <= viewRight))
                || !((obj.getY() + obj.getSprite()->getHeight() >= viewBottom) && (obj.getY() <= viewTop))
                && !updateOffscreen)
            {
                continue;
            }

            //Perform the collision check
            if (obj.willCollide(obj, frameTime, colpos1, colpos2))
            {
                obj.collided = true;
                obj.collided = true;

                //execute collision scripts
                for (size_t k = 0; k < obj.objectCollisionScripts.size(); k++)
                {
                    gameInstance.scriptVM.executeLine(obj.objectCollisionScripts[k] + "(map.objects[" + (int)(i+1) + "], map.objects[" + (int)(j+1) + "], " +
                        "Vector(" + colpos1.getX() + "," + colpos1.getY() + "), Vector(" + colpos2.getX() + "," + colpos2.getY() + ")"
                        + ")");
                }
                for (size_t k = 0; k < obj.objectCollisionScripts.size(); k++)
                {
                    gameInstance.scriptVM.executeLine(obj.objectCollisionScripts[k] + "(map.objects[" + (int)(j+1) + "], map.objects[" + (int)(i+1) + "], " +
                        "Vector(" + colpos2.getX() + "," + colpos2.getY() + "), Vector(" + colpos1.getX() + "," + colpos1.getY() + ")"
                        + ")");
                }
            }
        }

        if ((int)i > (int)(em.getObjectCount()-1))
        {
            return;
        }

        //check for collisions with the map
        gameInstance.scriptVM.executeLine(string("map.objects[") + (int)(i+1) + "][\"tilecollisions\"]={}");
        if (obj.willCollideWithMap(tilemap, frameTime))
        {
            for (size_t j = 0; j < obj.collidedTiles.size(); j++)
            {

                //gameInstance.scriptVM.executeLine(string("map.objects[") + (int)(i + 1) + "].tilecollisions[" + (int)(i + 1) + "]=" + 1);//string(obj.collidedTiles[j]));
                for (size_t k = 0; k < obj.collisionScripts.size(); k++)
                {
                    gameInstance.scriptVM.executeLine(obj.collisionScripts[k] +
                        "(map.objects[" + (int)(i+1) + "]," +
                        obj.collidedTiles[j].tilepos.getX() + "," +
                        obj.collidedTiles[j].tilepos.getY() + "," +
                        tilemap.getTileset()->getTile(tilemap.getTile(obj.collidedTiles[j].tilepos.getX(), obj.collidedTiles[j].tilepos.getY(),0)).block + "," +
                        obj.collidedTiles[j].colpos.getX() + "," +
                        obj.collidedTiles[j].colpos.getY()
                        + ")");
                }

            }
        }
    }

    //clean up objects that are marked for destruction
    for (size_t i = 0; i  < em.getObjectCount(); i++)
    {
        auto& obj = em.getObjectByIndex<PhysicalEntity>(i);
        if (obj.getDestructionFlag())
        {
            em.removeObjectByIndex(i);

            //rearrange script objects
            for (size_t j = i; j < em.getObjectCount(); j++)
            {
                gameInstance.scriptVM.executeLine(string("map.objects[") + (int)(j+1) + "]=map.objects[" + (int)(j+2) + "]");
                gameInstance.scriptVM.executeLine(string("map.objects[") + (int)(j+1) + "].id=" + (int)j);
            }
            gameInstance.scriptVM.executeLine(string("map.objects[") + (int)(em.getObjectCount() + 1) + "]=nil");
            i--;
        }
    }

    //execute the map's update scripts
    for (size_t i = 0; i < updateScripts.size(); i++)
    {
        gameInstance.scriptVM.executeLine(updateScripts[i] + "()");
    }
    */
    if (_createMapQueuedFlag)
    {
        createFromFile(_mapToCreate);
        _createMapQueuedFlag = false;
    }
}


//get-functions for the tilemap

int Map::getWidth()
{
    return _tilemap.getWidth();
}

int Map::getHeight()
{
    return _tilemap.getHeight();
}

int Map::getLayerCount()
{
    return _tilemap.getLayers();
}

//set a new background
void Map::setBackground(std::string textureName)
{
    if (textureName.length() > 0)
    {
        try
        {
            _background = _game.getTextureManager().requestResourcePtr(textureName.c_str())->resource;
        }
        catch (Exception &e)
        {
            _background = 0;
            clog << "Warning: Error loading background: " << e.what() << endl;
        }
    }
    else
    {
        _background = 0;
    }
    _backgroundName = textureName;
}

//set a new tileset
void Map::setTileset(string ts)
{
    if (ts.length() > 0)
    {
        try
        {
            _tilemap.setTileset(_game.getTilesetManager().requestResourcePtr(ts.c_str())->resource);
            _tilesetName = ts;
        }
        catch (Exception &e)
        {
            clog << "Warning: Error setting tilseset: " << e.what() << endl;
        }
    }
}

std::string Map::getTilesetName()
{
    return _tilesetName;
}

int Map::getTileSize() const
{
    return _tilemap.getTileSize();
}

unsigned int Map::getTile(int x, int y, int layer)
{
    return _tilemap.getTile(x, y, layer);
}

unsigned int Map::getTileAt(int x, int y, int layer)
{
	int tilesize = _tilemap.getTileSize();

	int xTile = (int)(x / tilesize);
	int yTile = (int)(y / tilesize);

    return _tilemap.getTile(xTile, yTile, layer);
}

void Map::setTileAt(double x, double y, int layer, int newTile)
{
	if (x < 0 || y < 0)
	{
		return;
	}

	int tilesize = _tilemap.getTileSize();

	int xTile = (int)(x / tilesize);
	int yTile = (int)(y / tilesize);


	_tilemap.setTile(xTile, yTile, layer, newTile);
}

void Map::setTile(int x, int y, int layer, int newTile)
{
    _tilemap.setTile(x, y, layer, newTile);
}

void Map::setFlag(std::string flag, bool value)
{
    if (!flag.compare("updateoffscreen"))
    {
        _updateOffscreen = value;
    }
}
const Tilemap& Map::getTilemap() const
{
    return _tilemap;
}


/*
    MapState class
*/

MapState::MapState(Game &game) : GameState(game), gamemap(game, *this)
{

}

MapState::~MapState()
{
    //make the global "map" invalid in the script
	gameInstance.scriptVM.executeLine("map=nil");
}

//Initialize scripts for map state
void MapState::initScript()
{
	luabind::globals(gameInstance.scriptVM.getVm())["map"] = &gamemap;
	gameInstance.scriptVM.executeLine("map[\"objects\"]={}");
}
