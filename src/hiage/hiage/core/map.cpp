/*!
	\file game_map.cpp

	\author Hallgeir Lien

	Description:
*/

#include <iostream>
#include <fstream>
#include "map.h"
#include "entitymanager.hpp"

#include "../lua-includes.h"

using namespace hiage;
using namespace std;

Map::Map(Game &game, GameState& gameState) : gameInstance(game), gameState(gameState)
{
    background = 0;
    objectDeletedFlag = false;
    updateOffscreen = false;

    //temp
    mapToCreate = "";
    createMapQueuedFlag = false;
}

Map::~Map()
{
    destroy();
}

const int Map::MAPVERSION = 100;


//sort the objects using the quicksort algorithm
std::vector<PhysicalEntity*> Map::sortObjects(const std::vector<PhysicalEntity*> & array)
{
    vector<PhysicalEntity*> less;
    vector<PhysicalEntity*> greater;
    vector<PhysicalEntity*> equal;

    if (array.size() <= 1)
    {
        return array;
    }

    double pivot = array[(int)(array.size() / 2)]->getX();

    for (size_t i = 0; i < array.size(); i++)
    {
        if (array[i]->getX() < pivot)
        {
            less.push_back(array[i]);
        }
        if (array[i]->getX() > pivot)
        {
            greater.push_back(array[i]);
        }
        if (array[i]->getX() == pivot)
        {
            equal.push_back(array[i]);
        }
    }

    less = sortObjects(less);
    less.insert(less.end(), equal.begin(), equal.end());
    greater = sortObjects(greater);
    less.insert(less.end(), greater.begin(), greater.end());

    return less;
}

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

	ifstream file(path.c_str());
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
	tilemap.createMap(width,height,layers,tilesize);

	tilemap.setTileset(gameInstance.getTilesetManager().requestResourcePtr(buffer)->resource);
	tilesetName = buffer;

	delete [] buffer;

	//read background name
	file.read((char*)&temp, 4);
	buffer = new char[temp+1];
	file.read(buffer, temp);
	buffer[temp] = 0;
    setBackground(buffer);
    backgroundName = buffer;

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
        includeScripts.push_back(string(buffer));
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
        initScripts.push_back(string(buffer));

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
        updateScripts.push_back(string(buffer));

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
        shutdownScripts.push_back(string(buffer));

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
				tilemap.setTile(x,y,z,tile);
			}
		}
	}

	//load objects
	file.read((char*)&temp, 4);
	if (temp < 0)
	{
		throw IOException("ERROR: Number of objects is less than 0.");
	}

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
		createObject(buffer, objx, objy, runScripts);

		delete [] buffer;
	}

	//include the script files
    for (unsigned int i = 0; i < includeScripts.size(); i++)
    {
        gameInstance.scriptVM.runFile(includeScripts[i]);
    }

    if (runScripts)
    {
        //run initialization scripts
        for (unsigned int i = 0; i < initScripts.size(); i++)
        {
            gameInstance.scriptVM.executeLine(initScripts[i] + "()");
        }
    }
#pragma warning(pop)
}

void Map::queueCreateMap(string path)
{
    mapToCreate = path;
    createMapQueuedFlag = true;
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
    tilemap.createMap(width, height, layers, tileSize);
}

/*

TODO - reimplement later

void Map::saveToFile(string path)
{
	clog << "Saving map to file " << path.c_str() << endl << flush;
	//string tileSet = "grassland"; //TODO: get the real tileset name
	int mapWidth = tilemap.getWidth();
	int mapHeight = tilemap.getHeight();
	int mapLayers = tilemap.getLayers();
	int temp;	//used to store string lengths etc.
	int tileSize = tilemap.getTileSize();

	//check for valid dimensions
	if (mapWidth <= 0 || mapHeight <= 0 || mapLayers <= 0)
	{
		throw Exception("ERROR: Invalid map dimensions.");
	}

	//open file
	ofstream file(path.c_str());
	if (!file.is_open())
	{
		throw IOException(string("ERROR: Could not load file ") + path);
	}

	//store version of the map
	file.write((char*)&MAPVERSION, 4);

	//write dimensions
	file.write((char*)&mapWidth, 4);
	file.write((char*)&mapHeight, 4);
	file.write((char*)&mapLayers, 4);
	file.write((char*)&tileSize, 4);

	//tileset name
	temp = tilesetName.length();
	file.write((char*)&temp, 4);
	file.write(tilesetName.c_str(), tilesetName.length());

	//background name
	temp = backgroundName.length();
	file.write((char*)&temp, 4);
	file.write(backgroundName.c_str(), backgroundName.length());

    //scripts
    //script files to include
    clog << "Saving script information..." << endl;
    temp = includeScripts.size();
    file.write((char*)&temp, 4);
    for (int i = 0; i < temp; i++)
    {
        int temp2 = includeScripts[i].length();
        file.write((char*)&temp2, 4);

        file.write(includeScripts[i].c_str(), temp2);
        clog << includeScripts[i].c_str() << endl;
    }

    //init scripts
    temp = initScripts.size();
    file.write((char*)&temp, 4);
    for (int i = 0; i < temp; i++)
    {
        int temp2 = initScripts[i].length();
        file.write((char*)&temp2, 4);

        file.write(initScripts[i].c_str(), temp2);
        clog << initScripts[i].c_str() << endl;
    }

    //update scripts
    temp = updateScripts.size();
    file.write((char*)&temp, 4);
    for (int i = 0; i < temp; i++)
    {
        int temp2 = updateScripts[i].length();
        file.write((char*)&temp2, 4);

        file.write(updateScripts[i].c_str(), temp2);
        clog << updateScripts[i].c_str() << endl;
    }

    //shutdown scripts
    temp = shutdownScripts.size();
    file.write((char*)&temp, 4);
    for (int i = 0; i < temp; i++)
    {
        int temp2 = shutdownScripts[i].length();
        file.write((char*)&temp2, 4);

        file.write(shutdownScripts[i].c_str(), temp2);
        clog << shutdownScripts[i].c_str() << endl;
    }

	//tile data
	//(writing in format: x1y1l1 x2y1l1 x3y1l1 ... x1y2l1 x2y2l1 x3y2l1 ... x1y1l2 x2y1l2 x3y1l2
	clog << "Saving tilemap..." << endl;
	for (int z = 0; z < mapLayers; z++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			for (int x = 0; x < mapWidth; x++)
			{
				int tile = tilemap.getTile(x, y, z);
				if (tilemap.getTile(x,y,z) < 0)
				{
					throw Exception(string("ERROR: Tile at (") + x + string(", ") + y + string(",") + z + ") is negative.");
				}
				file.write((char*)&tile, 4);
			}
		}
	}

	//object data (object name + position, and number of objects)
	temp = objects.size();

	file.write((char*)&temp, 4);
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		//object name
		string objectName = obj.getName();
		clog << "Saving entity " << objectName << endl;
		temp = objectName.size();
		file.write((char*)&temp, 4);
		file.write(objectName.c_str(), objectName.length());

		//position
		double objx = (int)obj.getX();
		double objy = (int)obj.getY();

		file.write((char*)&objx, 8);
		file.write((char*)&objy, 8);
	}


	file.close();

	clog << "Map saving complete.\n" << flush;
}*/



//destroy the map content
void Map::destroy()
{
    tilemap.destroy();

    includeScripts.clear();
    initScripts.clear();
    updateScripts.clear();
    shutdownScripts.clear();

    //reset scripting
    gameInstance.scriptVM.executeLine("map=nil");
    luabind::globals(gameInstance.scriptVM.getVm())["map"] = this;
	gameInstance.scriptVM.executeLine("map[\"objects\"]={}");
	background = 0;
}


//create a new object on the map
PhysicalEntity &Map::createObject(std::string name, double x, double y, bool runScripts)
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

PhysicalEntity &Map::createObject(std::string name, double x, double y)
{
    return createObject(name, x, y, true);
}



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
    Display &disp = gameInstance.getDisplay();
    Renderer &renderer = disp.getRenderer();
    double aspect = disp.getAspectRatio();
    double zoom = disp.getZoom();
    double camX = disp.getCamX();
    double camY = disp.getCamY();

    double viewLeft = camX - (zoom * aspect);
    double viewRight = camX + (zoom * aspect);
    double viewTop = camY + zoom;
    double viewBottom = camY - zoom;

    //render the objects
    auto& em = gameState.getEntityManager();
    for (size_t i = 0; i < em.getObjectCount(); i++)
	{
        auto& obj = em.getObjectByIndex<PhysicalEntity>(i);
	    //check if the object is inside the viewport
	    if ((obj.getX() + obj.getSprite()->getWidth() >= viewLeft) && (obj.getX() <= viewRight))
        {
            if ((obj.getY() + obj.getSprite()->getHeight() >= viewBottom) && (obj.getY() <= viewTop))
            {
                obj.render(renderer, ObjectZ::MIDDLE);
            }
        }
	}

    ObjectZ depth = ObjectZ::MIDDLE_BACK;

    //and the tilemap
	for (int i = 0; i < tilemap.getLayers(); i++)
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
	    tilemap.render(renderer, disp.getCamX(), disp.getCamY(), disp.getZoom(), disp.getAspectRatio(), depth, i);
	}

	//and the background
	//TODO: add support for choosing parallax options
    if (background)
    {
        float offset;
        offset = disp.getCamX() / 1.5;

        float startx = (disp.getCamX() - (float)disp.getViewWidth() - offset) / background->getWidth();
        float endx = startx + ((float)disp.getViewWidth() / (float)background->getWidth());
        float y = disp.getCamY() - (disp.getViewHeight() / 2);

        startx -= 1.0f;
        endx += 1.0f;

        renderer.beginRender(ObjectZ::BACK, background);
        for (int x = (int)startx; x <= (int)endx; x++)
        {
            renderer.addVertex((double)x * background->getWidth() + offset, (double)y + background->getHeight(), 0, 0);
            renderer.addVertex((double)x * background->getWidth() + background->getWidth() + offset, (double)y + background->getHeight(), 1, 0);
            renderer.addVertex((double)x * background->getWidth() + background->getWidth() + offset, y, 1, 1);
            renderer.addVertex((double)x * background->getWidth() + offset, y, 0, 1);

        }

        renderer.endRender();
    }
}

void Map::update(double frameTime)
{
    Display & disp = gameInstance.getDisplay();

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

    if (createMapQueuedFlag)
    {
        createFromFile(mapToCreate);
        createMapQueuedFlag = false;
    }
}


//get-functions for the tilemap

int Map::getWidth()
{
    return tilemap.getWidth();
}

int Map::getHeight()
{
    return tilemap.getHeight();
}

int Map::getLayerCount()
{
    return tilemap.getLayers();
}

//set a new background
void Map::setBackground(std::string textureName)
{
    if (textureName.length() > 0)
    {
        try
        {
            background = gameInstance.getTextureManager().requestResourcePtr(textureName.c_str())->resource;
        }
        catch (Exception &e)
        {
            background = 0;
        }
    }
    else
    {
        background = 0;
    }
    backgroundName = textureName;
}

//set a new tileset
void Map::setTileset(string ts)
{
     if (ts.length() > 0)
    {
        try
        {
            tilemap.setTileset(gameInstance.getTilesetManager().requestResourcePtr(ts.c_str())->resource);
            tilesetName = ts;
        }
        catch (Exception &e)
        {

        }
    }
}

std::string Map::getTilesetName()
{
    return tilesetName;
}

int Map::getTileSize() const
{
    return tilemap.getTileSize();
}

unsigned int Map::getTile(int x, int y, int layer)
{
    return tilemap.getTile(x, y, layer);
}

unsigned int Map::getTileAt(int x, int y, int layer)
{
	int tilesize = tilemap.getTileSize();

	int xTile = (int)(x / tilesize);
	int yTile = (int)(y / tilesize);

    return tilemap.getTile(xTile, yTile, layer);
}

void Map::setTileAt(double x, double y, int layer, int newTile)
{
	if (x < 0 || y < 0)
	{
		return;
	}

	int tilesize = tilemap.getTileSize();

	int xTile = (int)(x / tilesize);
	int yTile = (int)(y / tilesize);


	tilemap.setTile(xTile, yTile, layer, newTile);
}

void Map::setTile(int x, int y, int layer, int newTile)
{
    tilemap.setTile(x, y, layer, newTile);
}

void Map::setFlag(std::string flag, bool value)
{
    if (!flag.compare("updateoffscreen"))
    {
        updateOffscreen = value;
    }
}



/*
    MapState class
*/

MapState::MapState(Game &game) : GameState(game), gamemap(game, *this)
{
    auto sysFactory = getSystemsFactory();
    systems.push_back(sysFactory.createSystem("movement"));
    systems.push_back(sysFactory.createSystem("objectrendering"));
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
