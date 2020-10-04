/*!
	\file game_map.h
	\author Hallgeir Lien

	Description:
*/

#pragma once

#include <string>
#include <vector>

#include "entity.hpp"
#include "../gfx/tilemap.hpp"
#include "game.hpp"

namespace hiage
{
	class Game;
	class Map
	{
	private:
        static const int MAPVERSION;

		Game &                          _game;	//reference to the game instance
		GameState&						_gameState;
		Tilemap                         _tilemap;
		Texture *                       _background;

		std::string                     _tilesetName;
		std::string                     _backgroundName;
		bool                            _objectDeletedFlag;

        //some map flags
        bool                            _updateOffscreen;
        bool                            _createMapQueuedFlag;
        std::string                     _mapToCreate;            //We can't create a map directly in the script because
                                                                //that would mess up stuff when objects are destroyed
                                                                //while their scripts are running.
                                                                //So we queue the creation for after the scripts.

	private:
        //scripts that should be run during loading, updating or shutdown of map
        std::vector<std::string>        _includeScripts;     //script files to run before any other scripts are run
		std::vector<std::string>        _initScripts;
		std::vector<std::string>        _updateScripts;
		std::vector<std::string>        _shutdownScripts;

	public:
		Map(Game &game, GameState& gameState);
		~Map();

		// TODO - reimplement later
		//void            deleteObjectAt(double x, double y);

		//map creation/loading/saving
		void            loadFromJson(std::string path, bool runScripts);
        void            createEmpty(int width, int height, int layers, int tileSize, bool onlyTiles = false);
        void            saveAsJson(std::string path);
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

		const Tilemap&	getTilemap() const;
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

