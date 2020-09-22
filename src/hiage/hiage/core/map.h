/*!
	\file game_map.h
	\author Hallgeir Lien

	Description:
*/

#pragma once

#include <string>
#include <vector>

#include "game.h"
#include "entity.h"
#include "../gfx/tilemap.h"

namespace Hinage
{
	class Map
	{
	private:
        static const int MAPVERSION;

		typedef std::vector<PhysicalEntity *>   ObjectList;

		ObjectList                      objects;        //all created objects
		Game &                          gameInstance;	//reference to the game instance
		Tilemap                         tilemap;
		Texture *                       background;

		std::string                     tilesetName;
		std::string                     backgroundName;
		bool                            objectDeletedFlag;

        //some map flags
        bool                            updateOffscreen;
        bool                            createMapQueuedFlag;
        std::string                     mapToCreate;            //We can't create a map directly in the script because
                                                                //that would mess up stuff when objects are destroyed
                                                                //while their scripts are running.
                                                                //So we queue the creation for after the scripts.
    private:
        //sort objects based on X-position
        std::vector<PhysicalEntity*>    sortObjects(const std::vector<PhysicalEntity*> & array);

    public:
        //scripts that should be run during loading, updating or shutdown of map
        std::vector<std::string>        includeScripts;     //script files to run before any other scripts are run
		std::vector<std::string>        initScripts;
		std::vector<std::string>        updateScripts;
		std::vector<std::string>        shutdownScripts;

	public:
		Map(Game &game);
		~Map();

		/*!
			Create a new object to add to the map. Runs initialization scripts.
		*/
		PhysicalEntity &createObject(std::string name, double x, double y);
		PhysicalEntity &createObject(std::string name, double x, double y, bool runScripts);
		PhysicalEntity &getObject(int index);
		PhysicalEntity &getObjectAt(double x, double y);
		size_t          getObjectCount();
		void            deleteObject(int index);
		void            deleteObjectAt(double x, double y);

		//map creation/loading/saving
		void            createFromFile(std::string path);
		void            createFromFile(std::string path, bool runScripts);
        void            createEmpty(int width, int height, int layers, int tileSize, bool onlyTiles = false);
        void            saveToFile(std::string path);
        void            destroy();

        void            queueCreateMap(std::string path);

        //update and render the map's objects and stuff
        void            update(double frameTime);
		void            render();

        //a few functions to get the properties of the tilemap
        int             getLayerCount();
        int             getWidth();
        int             getHeight();

        //set the background
        void            setBackground(std::string textureName);

        //set the tileset
        void            setTileset(std::string tilesetName);
        std::string     getTilesetName();
        int             getTileSize() const;

        //set/get a single tile
        unsigned int    getTile(int x, int y, int layer);
        unsigned int    getTileAt(int x, int y, int layer);
        void            setTile(int x, int y, int layer, int newTile);
        void            setTileAt(double x, double y, int layer, int newTile);

        void            setFlag(std::string flag, bool value);


	};

	/*!
		An implementation of GameState, used for maps. Contains initialization of script globals for maps and such.
		See the class Engine::GameState for details on members.
	*/
	class MapState : public GameState
	{
	protected:
		Map gamemap;

	public:
		MapState(Game &game);

		virtual ~MapState();
		virtual void initScript();
	};

}

