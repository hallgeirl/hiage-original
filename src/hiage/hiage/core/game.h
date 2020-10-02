/*!
	\file com_game.h

	\author Hallgeir Lien

	Contains declaration of the Game and GameState class.
*/

#pragma once

#include <vector>
#include <iostream>

//engine headers
#include "../gfx/display.h"
#include "../gfx/particles.h"

#include "inputmanager.h"
#include "resourcemanager.hpp"
#include "typedefs.h"
#include "audio.h"
#include "script_lua.h"
#include "../util/timer.h"
#include "gamestate.hpp"

namespace hiage
{
	class GameState;

	/*!
		Game base class.
		Any game using this engine makes an instance of an inherited Game class.
		This class handles the initialization of all subsystems like video, audio  and so on,
		and it stores all the loaded resources in the resource managers. The member function
		onInit() needs to be inherited and implemented by the users of the engine.
	*/
	class __IMPORTEXPORT Game
	{

	protected:
		Display			display;	//!< Main screen object.
		InputManager	input;		//!< Used to handle input.
		AudioManager	audio;		//!< All sounds and music are stored and handled here.
        double          timeFactor;
		double			framerateLimit;
		double			lastFrameTime;
		//resources
		TextureManager	textureManager;	//!< Stores the loaded textures.
		SpriteManager	spriteManager;	//!< Stores the loaded sprites.
		TilesetManager	tilesetManager;	//!< Stores the loaded tilesets.
		FontManager     fontManager;	//!< Stores all loaded fonts.
		ObjectManager	objectManager;  //!< Stores all loaded objects.
		std::map<std::string, std::string>		objectList;		//!< Contains the path to all XML files that is loaded, with the object name associated with each path.
		Timer           gameTimer;       //!< The elapsed time since the game class was initialized.
		bool running;

		std::vector<GameState *> states;	//!< The game states. The game state can be changed with setGameState, pushState and popState.

	protected:
		/*!
			onInit is called after the game object has been initialized.
			This particular function MUST be inherited and implemented by the user of this engine.
		*/
		virtual void onInit() {};

		/*!
			Loads all the XML resources in the specified directory of the specified resource type. It works recursively.
			\param dir The directory to load the resources from.
			\param resType The type of resource to load. This can be any of the constants defined in the ResourceTypeEnum enumeration, defined in com_typedefs.h.
		*/
		void loadResources(std::string dir, ResourceTypeEnum resType);

	public:
		LuaVM scriptVM; //!< The LUA script's virtual machine class

	public:
		Game(double framerateLimit);
		virtual ~Game();

		/*!
			Initialize the display (screen), audio, input.
			\param width Screen width.
			\param height Screen height.
			\param fullscreen Set to true if you want to run in fullscreen.

			\throw FatalException If initialization of any of the subsystems fails, the program probably can't continue and a FatalException is thrown.
		*/
		void initialize(int width, int height, bool fullscreen);

		/*!
			Runs the game. This should be called every time the main game loop loops.
			\param frameTime The amount of time used to render the last frame, in seconds.
		*/
		void run(bool doEvents = true);

        void setTimeFactor(double value);
        double getTimeFactor();

		bool isRunning();
		void stop();

		Font &createFont(std::string font);
		void printText(Font & font, std::string text, double x, double y, double scale = 1, double spacing = 0);

        void drawTexture(std::string texname, double x, double y);

		/*!
			Shuts the game down. De-initializes everything and frees any used resources. Be sure to not call run() after this.
		*/
		void shutdown();

		/*!
			Removes any existing game state and sets a new one.
			\param state A pointer to an derived GameState object you want to set.
		*/
		void setGameState(GameState * state);

		/*!
			Pushes a new game state onto the stack of states. This pauses the last running state.
			\param state A pointer to an derived GameState object you want to push onto the stack.
		*/
		void pushState(GameState * state);

		/*!
			Pops the topmost (currently running) game state. This resumes the next-topmost state, and deletes the previously topmost state.
		*/
		void popState();

		//! Returns a reference to the display object.
		Display &           getDisplay();
		//! Returns a reference to the inputManager object.
		InputManager &      getInputManager();
		//! Returns a reference to the audioManager object.
		AudioManager &      getAudioManager();
		//! Returns a reference to the textureManager object.
		TextureManager &    getTextureManager();
		//! Returns a reference to the spriteManager object.
		SpriteManager &     getSpriteManager();
		//! Returns a reference to the tilesetManager object.
		TilesetManager &    getTilesetManager();
		//! Returns a reference to the fontManager object.
		FontManager &       getFontManager();
		//! Returns a reference to the object list.
		ObjectList &        getObjectList(); // TODO - remove this once migrated to ObjectManager for object loading
		
		ObjectManager &		getObjectManager();

		/*!
			Returns the XML file name containing the object's properties associated with the specified name.
			\param name Name of the object. The name of an object is specified in it's XML file.
			\return The path of the XML file containing the object data.
		*/
		const std::string&         getObjectFile(std::string name) const;

		/*!
			Returns the name of the sprite associated with the object with the specified name.
			\param name Name of the object. The name of an object is specified in it's XML file.
			\return The name of the sprite associated with the object.
		*/
		std::string         getObjectSprite(std::string name);
	};
}
