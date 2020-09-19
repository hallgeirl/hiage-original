#include <iostream>
#include <math.h>
#include "objectpalette.h"

using namespace Engine;
using namespace std;

ObjectPalette::ObjectPalette()
{
    visible = false;
    initialized = false;
    displaySize = 32;
}

ObjectPalette::~ObjectPalette()
{
    for (int i = 0; i < objects.size(); i++)
    {
        delete objects[i];
    }
}

//create the object palette
bool ObjectPalette::create(Game *game, float width, float height, Engine::Vector2<double> position, int displaySize)
{
    clog << "Creating object palette...\n" << flush;

    this->position = position;
    this->width = width;
    this->height = height;
    this->displaySize = displaySize;

    ObjectList & objList = game->getObjectList();

    ObjectList::iterator itr = objList.begin();

    float x = 0, y = -32;
    int counter = 0;
    int objectsPerRow = width / displaySize;

	//iterate through the object list and add objects as they come
    while (itr != objList.end())
    {
        clog << "Creating object " << itr->first << " from file " << itr->second << "...\n" << flush;

        SpriteManager::Resource * sprite = game->getSpriteManager().requestResourceCopy(game->getObjectSprite(itr->first).c_str());
        if (!sprite)
        {
            cerr << "ERROR: Could not get sprite. Aborting.\n" << flush;
            return false;
        }

        TextureManager::Resource * texture = game->getTextureManager().requestResourcePtr(sprite->strData1.c_str());
        if (!texture)
        {
            cerr << "ERROR: Could not get texture. Aborting.\n" << flush;
            return false;
        }

        sprite->resource->create(texture->resource, sprite->intData1, sprite->intData2);

        PhysicalEntity * tempObject = new PhysicalEntity;
        tempObject->createFromFile(itr->second, sprite->resource);

        objects.push_back(tempObject);
        //objects[objects.size()-1]->setPosition(game->getDisplay().environmentToWindow(position));

        counter ++;
        objects[objects.size()-1]->setPosition(Vector2<double>(x,y) + position);

        x += displaySize;
        if (counter % objectsPerRow == 0)
        {
            y -= displaySize;
            x = 0;
        }

        ++itr;
    }

    visible = true;
    initialized = true;

    clog << "OK: Object palette created successfully.\n" << flush;

    return true;
}

void ObjectPalette::render(Game *game, Font *font)
{
    if (!visible || !initialized)
    {
        return;
    }

	Renderer &renderer = game->getDisplay().getRenderer();

	Vector2<double> pPosition = game->getDisplay().environmentToWindow(position);


	glPushMatrix();

	glTranslatef((float)pPosition.getX(), (float)pPosition.getY(), 0);

	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);

	glColor4f(1,1,1,1);

	glBegin(GL_LINES);
	//upper left
	glVertex3f(0.0f, 0.0f, 0.1f);
	//upper right
	glVertex3f(width, 0.0f, 0.1f);

	//upper right
	glVertex3f(width, 0.0f, 0.1f);
	//lower right
	glVertex3f(width, -height, 0.1f);

	//lower right
	glVertex3f(width, -height, 0.1f);
	//lower left
	glVertex3f(0.0f, -height, 0.1f);

	//lower left
	glVertex3f(0.0f, -height, 0.1f);
	//upper left
	glVertex3f(0.0f, 0.0f, 0.1f);
	glEnd();

	//draw tiles
	//find the number of textures that can fit horizontally
	int horizontalObjects = (int)floor(width / displaySize);
	int verticalObjects = (int)floor(height / displaySize);

	//calculate the spacing between each tile
	float xOffset = ((int)width % displaySize) / (float)horizontalObjects;
	float yOffset = ((int)height % displaySize) / (float)verticalObjects;

	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);

	int x = 0, y = -32;
	int objectsPerRow = width / displaySize;

    for (int i = 0; i < objects.size(); i++)
    {
    	objects[i]->setPosition(Vector2<double>(x,y) + pPosition);
    	objects[i]->render(renderer, Renderer::FRONT/*, displaySize*/);

    	x += displaySize;
    	if ((i+1) % objectsPerRow == 0)
    	{
    		y -= displaySize;
    		x = 0;
    	}
    }

	if (font)
	{
	    font->renderText(renderer, Renderer::CLOSEST, "Objects", Vector2<double>(0,0) + pPosition, 0.8);
	}

	glPopMatrix();
}

int ObjectPalette::getObjectFromInput(Engine::Game *game, Engine::Vector2<double> mousePosition)
{
    if (!visible)
    {
        return -1;
    }

	Vector2<double> position = game->getDisplay().environmentToWindow(this->position);


	//find the number of textures that can fit horizontally
	int cols = width / displaySize;
	int rows = height / displaySize;

	//calculate the spacing between each tile
/*	float xOffset = ((int)width % tileDisplaySize) / (float)horizontalTiles;
	float yOffset = ((int)height % tileDisplaySize) / (float)verticalTiles;*/

	for (int y = 0; y < rows; y++)
	{
		float yPosition = -(y * displaySize) + position.getY();

		for (int x = 0; x < cols; x++)
		{

			float xPosition = (x * displaySize) + position.getX();

			float left = xPosition;
			float right = xPosition + displaySize;
			float top = yPosition;
			float bottom = yPosition - displaySize;

			if (mousePosition.isInRect(left, top, right, bottom))
			{
			    cout << "Selecting object #" << (y*cols+x) << endl;
				return (y * cols + x);
			}
		}
	}


	return -1;
}

void ObjectPalette::hide()
{
    visible = false;
}

void ObjectPalette::show()
{
    visible = true;
}

string ObjectPalette::getObjectName(int index)
{
    if (index >= 0 && index < objects.size())
    {
        return objects[index]->getName();
    }

    return "";
}
