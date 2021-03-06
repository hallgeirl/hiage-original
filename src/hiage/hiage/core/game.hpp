/*!
	\file game.hpp

	\author Hallgeir Lien

	Contains declaration of the Game and GameState class.
*/

#pragma once

#include <vector>
#include <iostream>

//engine headers
#include "../gfx/display.h"
#include "../gfx/particles.h"
#include "../gfx/spritecontroller.hpp"

#include "debugrenderer.hpp"
#include "inputmanager.hpp"
#include "resourcemanager.hpp"
#include "typedefs.h"
#include "audio.h"
#include "script_lua.h"
#include "../util/timer.h"
#include "gamestate.hpp"
#include "gameconfig.hpp"


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
		SpriteController spriteController;
		DebugRenderer*  _debugRenderer; // For rendering e.g. texts and visual artifacts meant for debugging.
		Timer           gameTimer;       //!< The elapsed time since the game class was initialized.
		bool running;

		std::string		dataRoot;		// Path to the data directory for the game's assets
		std::vector<GameState *> states;	//!< The game states. The game state can be changed with setGameState, pushState and popState.

		Font 			_consoleFont;
		void loadResourcesRecursive(std::string dir, ResourceTypeEnum resType);
	protected:
		/*!
			onInit is called after the game object has been initialized.
			This particular function MUST be inherited and implemented by the user of this engine, and this is where you should do all resource loading.
		*/
		virtual void onInit() = 0;

		/*!
			Loads all the XML resources in the specified directory of the specified resource type. It works recursively.
			\param dir The directory to load the resources from.
			\param resType The type of resource to load. This can be any of the constants defined in the ResourceTypeEnum enumeration, defined in com_typedefs.h.
		*/
		void loadResources(std::string dir, ResourceTypeEnum resType);

	public:
		LuaVM scriptVM; //!< The LUA script's virtual machine class

	public:
		Game(double framerateLimit, const KeyBindings& keyBindings, const std::string& dataRoot);
		Game(const Game&) = delete;

		virtual ~Game();

		/*!
			Initialize the display (screen), audio, input.
			\param width Screen width.
			\param height Screen height.
			\param fullscreen Set to true if you want to run in fullscreen.

			\throw FatalException If initialization of any of the subsystems fails, the program probably can't continue and a FatalException is thrown.
		*/
		void initialize(const GameConfig& gameConfig);

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
		void printText(const Font& font, const std::string& text, double x, double y, double scale = 1, double spacing = 0);
		void printTextFixed(const Font& font, const std::string& text, double x, double y, ScreenHorizontalPosition horizontalPos, ScreenVerticalPosition verticalPos, double scale = 1, double spacing = 0);

		void enableDebugRendering(DebugRenderer* debugRenderer);

        void drawTexture(std::string texname, double x, double y);

		std::string getResourcePath(const std::string& relativePath) const;

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
		const TextureManager&	 getTextureManager() const;
		//! Returns a reference to the spriteManager object.
		const SpriteManager&     getSpriteManager() const;
		//! Returns a reference to the tilesetManager object.
		TilesetManager &    getTilesetManager();
		//! Returns a reference to the fontManager object.
		FontManager &       getFontManager();
		//! Returns a reference to the object list.
		const ObjectManager & getObjectManager() const;

		const SpriteController& getSpriteController() const;

		// Returns the on-screen debug writer
		DebugRenderer* getDebugRenderer();
	};
}
