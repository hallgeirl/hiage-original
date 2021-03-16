/*!
	\file game.cpp

	\author Hallgeir Lien

	Contains implementation of the Game class.
*/

#include <iostream>

#include "game.hpp"
#include "entitymanager.hpp"
#include "../util/exceptions.h"
#include "../sdl-includes.h"
#include <filesystem>
#include <thread>
#include <chrono>

#pragma warning(push, 0)
#include <tinyxml.h>
#pragma warning(pop)
#include "../lua-includes.h"


using namespace hiage;
using namespace std;
using namespace std::filesystem;

Game::Game(double framerateLimit, const KeyBindings& keyBindings, const std::string& dataRoot) 
	: gameTimer(true), lastFrameTime(0.05), framerateLimit(framerateLimit), dataRoot(dataRoot),
	scriptVM(dataRoot), input(keyBindings), audio(dataRoot), textureManager(dataRoot), spriteManager(dataRoot), objectManager(dataRoot), tilesetManager(textureManager, dataRoot), fontManager(dataRoot), spriteController(*this), _debugRenderer(nullptr)
{
	running = false;
	timeFactor = 1;
}

Game::~Game()
{
	shutdown();
	for (unsigned int i = 0; i < states.size(); i++)
	{
		delete states[i];
	}

	states.clear();

	if (_debugRenderer != nullptr)
		delete _debugRenderer;
}

//traverse a specified directory and load the resources in it
void Game::loadResources(std::string dir, ResourceTypeEnum resType)
{
	loadResourcesRecursive(getResourcePath(dir), resType);
}

void Game::loadResourcesRecursive(std::string dir, ResourceTypeEnum resType)
{
	directory_iterator end_itr;

	clog << "Loading resources from directory: " << dir << endl << flush;;

	try
	{
		for (const auto& itr : recursive_directory_iterator(dir))
		{
			if (itr.is_directory())
			{
				loadResources(itr.path().string(), resType);
			}
			else
			{
				switch (resType)
				{
                    case ResourceTypeEnum::SPRITE:
                    {
                        spriteManager.load(itr.path().string(), "");
                        break;
                    }

                        //case Object:
                    case ResourceTypeEnum::TEXTURE:
                    {
                        textureManager.load(itr.path().string(), "");
                        break;
                    }

                    case ResourceTypeEnum::TILESET:
                    {
                        tilesetManager.load(itr.path().string(), "");
                        break;
                    }

                    case ResourceTypeEnum::FONT:
                    {
                        fontManager.load(itr.path().string(), "");
                        break;
                    }

                    case ResourceTypeEnum::TILE:
                    {
                        break;
                    }
					case ResourceTypeEnum::OBJECT:
					{
						objectManager.load(itr.path().string(), "");
						break;
					}
				}
			}
		}
	}
	catch (std::exception e)
	{
		throw IOException(string("ERROR: ") + e.what());
	}

	clog << "OK: Finished loading resources from directory " << dir << endl << flush;
}

void Game::initialize(const GameConfig& gameConfig)
{
	clog << "Initializing game engine...\n" << flush;
	//initialize display
	display.initialize(gameConfig.display.displayWidth, gameConfig.display.displayHeight, gameConfig.display.fullscreen, gameConfig.display.vsync);

	display.setZoom(200.0);
	display.setState(DisplayState::DS_STRETCH_SCENE, false);
	display.setCamPosition(0.0f,0.0f);

	if (!audio.initialize(44100, 16))
	{
		throw FatalException("ERROR: Audio initialization failed.");
	}

	//initialize input
	if (!input.initialize())
	{
		throw FatalException("ERROR: Input initialization failed.");
	}

	clog << "OK: Game engine initialized properly.\n" << flush;

	onInit();

	_consoleFont = createFont(gameConfig.consoleFontName);
	_debugRenderer = new DebugRenderer(display, _consoleFont);
	_debugRenderer->setDebugFlags(gameConfig.debug.debugFlags);

	if (gameConfig.debug.enabled)
	{
		_debugRenderer->enable();
	}

	running = true;
}

void Game::pushState(GameState * state)
{
	if (!states.empty())
	{
		states.back()->pause();
	}

	states.push_back(state);
	states.back()->initScript();
	states.back()->init();
}

void Game::popState()
{
	if (states.empty())
	{
		return;
	}

	states.back()->destroy();

	states.pop_back();

	if (!states.empty())
	{
		states.back()->resume();
	}
}

void Game::setGameState(GameState * state)
{
	if (!states.empty())
	{
		//remove the previous state
		states.back()->destroy();
		states.pop_back();
	}


	states.push_back(state);
	states.back()->initScript();
	states.back()->init();
}


void Game::run(bool doEvents)
{
	static Timer frameTimer(true);		 //!< Used for frame limiting.
	
	frameTimer.reset();

	display.beginRender();

    SDL_Event event;

    if (doEvents)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                stop();
                return;
            }
        }
    }

	if (!states.empty())
	{
		states.back()->handleEvents(lastFrameTime);
		states.back()->update(lastFrameTime);
		states.back()->render();
		states.back()->cleanupFrame();
	}

	display.render();

	glDisable(GL_TEXTURE_2D);

	//the cross in the middle of the map
	glBegin(GL_LINES);
	//vertical
	glVertex2f(0.0f,20.0f);
	glVertex2f(0.0f,-20.0f);
	//horizontal
	glVertex2f(20.0f,0.0f);
	glVertex2f(-20.0f,0.0f);

	glEnd();

	glEnable(GL_TEXTURE_2D);

	/*
	* Limit framerate
	*/
	double frameTimeLimitMicroseconds = 1000000. / framerateLimit;
	if (framerateLimit > 0)
	{
		double frameTimeMicroseconds = frameTimer.getTime() * 1000000;
		long microsecondsToSleep = (long)(frameTimeLimitMicroseconds - frameTimeMicroseconds) - 1000;

		if (microsecondsToSleep > 1)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(microsecondsToSleep));
		}
	}

	static double accumulatedTime = 0;
	accumulatedTime += frameTimer.getTime();

	if (accumulatedTime >= 1)
	{
		cout << "FPS: " << (1 / frameTimer.getTime()) << " frametime " << frameTimer.getTime() << endl;
		accumulatedTime = 0;
	}

	lastFrameTime = std::min(frameTimer.getTime(), 0.02) * timeFactor;
	scriptVM.executeLine(string("frametime=") + lastFrameTime);
}

double Game::getTimeFactor()
{
    return timeFactor;
}

void Game::setTimeFactor(double value)
{
    if (value <= 0)
	{
	    timeFactor = 1;
	}
	else if (value > 1000)
	{
	    timeFactor = 1000;
	}
	else
	{
	    timeFactor = value;
	}
}



bool Game::isRunning()
{
	return running;
}

void Game::stop()
{
	running = false;
}

//cleanup
void Game::shutdown()
{
	//TODO: Clean up allocated resources.

}

Font & Game::createFont(std::string font)
{
    FontFactory ff;
    Font * f = ff.createFont(font, this);

    //f->renderText(display.getRenderer(), Renderer::CLOSEST, text, Vector2<double>(x,y), scale, spacing);

    return *f;
}

void Game::printText(const Font& font, const std::string& text, double x, double y, double scale , double spacing)
{
    font.renderText(display.getRenderer(), text, Vector2<double>(x,y), scale, spacing);
}

void hiage::Game::printTextFixed(const Font& font, const std::string& text, double x, double y, ScreenHorizontalPosition horizontalPos, ScreenVerticalPosition verticalPos, double scale, double spacing)
{
	auto& disp = getDisplay();
	double xPos = disp.getCamX() + x;// - disp.getZoom() * disp.getAspectRatio();
	double yPos = disp.getCamY() + y;// - disp.getZoom();

	switch (horizontalPos)
	{
	case ScreenHorizontalPosition::Left:
		xPos -= disp.getZoom() * disp.getAspectRatio();
		break;
	case ScreenHorizontalPosition::Right:
		xPos += disp.getZoom() * disp.getAspectRatio();
		break;
	}

	switch (verticalPos)
	{
	case ScreenVerticalPosition::Bottom:
		yPos -= disp.getZoom();
		break;
	case ScreenVerticalPosition::Top:
		yPos += disp.getZoom() - font.getCharacterHeight() * scale;
		break;
	}

	font.renderText(display.getRenderer(), text, Vector2<double>(xPos, yPos), scale, spacing);
}

void Game::drawTexture(std::string texname, double x, double y)
{
    Renderer & renderer = display.getRenderer();
    const Texture& tex = textureManager.requestResourceRef(texname.c_str()).resource;

    renderer.beginRender(ObjectZ::CLOSEST, &tex);

    renderer.addVertex(x, y, 0, 1);
    renderer.addVertex(x + tex.getWidth(), y, 1, 1);
    renderer.addVertex(x + tex.getWidth(), y + tex.getHeight(), 1, 0);
    renderer.addVertex(x, y + tex.getHeight(), 0, 0);

    renderer.endRender();
}

Display & Game::getDisplay()
{
    return display;
}

InputManager & Game::getInputManager()
{
    return input;
}

AudioManager & Game::getAudioManager()
{
    return audio;
}

const TextureManager& Game::getTextureManager() const
{
    return textureManager;
}
const SpriteManager& Game::getSpriteManager() const
{
    return spriteManager;
}

TilesetManager & Game::getTilesetManager()
{
    return tilesetManager;
}

FontManager & Game::getFontManager()
{
    return fontManager;
}

const ObjectManager& hiage::Game::getObjectManager() const
{
	return objectManager;
}

const SpriteController& hiage::Game::getSpriteController() const
{
	return spriteController;
}

std::string hiage::Game::getResourcePath(const std::string& relativePath) const
{
	std::filesystem::path root = dataRoot;
	auto fullPath = root / relativePath;

	return fullPath.string();
}

DebugRenderer* Game::getDebugRenderer()
{
	return _debugRenderer;
}