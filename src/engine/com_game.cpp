/*!
	\file com_game.cpp

	\author Hallgeir Lien

	Contains implementation of the Game class.
*/

#include <iostream>
//#include <boost/filesystem.hpp>

#include "com_game.h"
#include "com_exceptions.h"
#include "tinyxml/tinyxml.h"
#include "com_objectfactory.h"

#include <filesystem>

using namespace Hinage;
using namespace std::filesystem;

Game::Game() : gameTimer(true)
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
}



//using namespace boost::filesystem;

//traverse a specified directory and load the resources in it
void Game::loadResources(std::string dir, ResourceTypeEnum resType)
{
	directory_iterator end_itr;

	clog << "Loading resources from directory: " << dir << endl << flush;;

	try
	{
		for (const auto& itr : recursive_directory_iterator(dir))
		//for (directory_iterator itr(dir); itr != end_itr; itr++)
		{
			if (itr.is_directory())
			{
				loadResources(itr.path().string(), resType);
			}
			else
			{
				switch (resType)
				{
                    case SPRITE:
                    {
                        spriteManager.load(itr.path().string().c_str());
                        break;
                    }

                        //case Object:
                    case TEXTURE:
                    {
                        textureManager.load(itr.path().string().c_str());
                        break;
                    }

                    case TILESET:
                    {
                        tilesetManager.load(itr.path().string().c_str());
                        break;
                    }

                    case FONT:
                    {
                        fontManager.load(itr.path().string().c_str());
                        break;
                    }

                    case OBJECT:
                    {
                        //load the xml file

                        string temp = itr.path().string();
                        TiXmlDocument xmlDoc;
                        xmlDoc.LoadFile(temp.c_str());

                        if (!xmlDoc.LoadFile())
                        {
                            continue;
                        }

                        TiXmlElement * objectElement = 0;

                        objectElement = xmlDoc.FirstChildElement("object");
                        if (objectElement)
                        {
                            std::string tempName = objectElement->Attribute("name");
                            if (tempName.length())
                            {
                                objectList[tempName] = itr.path().string();
                            }
                        }
                        break;
                    }

                    case TILE:
                    {
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

void Game::initialize(int width, int height, bool fullscreen)
{
	clog << "Initializing game engine...\n" << flush;
	//initialize display
	display.initialize(width, height, fullscreen);

	display.setZoom(200.0);
	display.setState(Display::DS_STRETCH_SCENE, false);
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



#pragma warning TODO
	/*luabind::globals(scriptVM.getVm())["game"] = this;
	luabind::globals(scriptVM.getVm())["screen"] = &display;
	luabind::globals(scriptVM.getVm())["gametimer"] = &gameTimer;
	luabind::globals(scriptVM.getVm())["input"] = &input;
	luabind::globals(scriptVM.getVm())["audio"] = &audio;*/

	running = true;
	onInit();
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

void Game::run(double frametime, bool doEvents)
{
	display.beginRender();

	frametime *= timeFactor;

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
		states.back()->handleEvents(frametime);
		states.back()->update(frametime);
		states.back()->render(frametime);
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

	//scriptVM.executeLine(string("frametime=") + frametime);
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

void Game::printText(Font & font, std::string text, double x, double y, double scale , double spacing)
{
    font.renderText(display.getRenderer(), text, Vector2<double>(x,y), scale, spacing);
}

void Game::drawTexture(std::string texname, double x, double y)
{
    Renderer & renderer = display.getRenderer();
    Texture * tex = textureManager.requestResourcePtr(texname.c_str())->resource;

    renderer.beginRender(Renderer::CLOSEST, tex);

    renderer.addVertex(x, y, 0, 1);
    renderer.addVertex(x + tex->getWidth(), y, 1, 1);
    renderer.addVertex(x + tex->getWidth(), y + tex->getHeight(), 1, 0);
    renderer.addVertex(x, y + tex->getHeight(), 0, 0);

    renderer.endRender();
}


std::string Game::getObjectSprite(std::string name)
{
	std::string sprite = "";

	//load the xml file
	TiXmlDocument xmlDoc(objectList[name].c_str());
	if (!xmlDoc.LoadFile())
	{
		return sprite;
	}

	TiXmlElement *	objectElement = 0;
	TiXmlElement *	spriteElement = 0;

	objectElement = xmlDoc.FirstChildElement("object");

	//check if it's an object file
	if (!objectElement)
	{
		return sprite;
	}

	//store the sprite name
	spriteElement = objectElement->FirstChildElement("sprite");
	sprite = spriteElement->Attribute("name");

	return sprite;
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

TextureManager & Game::getTextureManager()
{
    return textureManager;
}
SpriteManager & Game::getSpriteManager()
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

ObjectList & Game::getObjectList()
{
    return objectList;
}

std::string Game::getObjectFile(std::string name)
{
    return objectList[name];
}

/*
    Gamestate class
*/

GameState::GameState(Game &game) : gameInstance(game)
{

}

GameState::~GameState()
{
}

void GameState::changeState(Game * game, GameState * state)
{
	std::clog << "Game state: Changed state.\n" << std::flush;
	game->setGameState(state);
}
