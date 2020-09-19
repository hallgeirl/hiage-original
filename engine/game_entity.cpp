/*
File: entity.cpp

Author: Hallgeir Lien

Description: Implementation of PhysicalEntity

Copyright (C) Hallgeir Lien - 2008
*/

#include "com_exceptions.h"
#include "tinyxml/tinyxml.h"
#include "game_entity.h"

using namespace Hinage;

PhysicalEntity::PhysicalEntity()
{
	sprite = 0;
	destroyFlag = false;
	position.set(0,0);
	frameTime = 0.005;
	canCollide = true;
	hFlip = false;
	vFlip = false;
}

PhysicalEntity::~PhysicalEntity()
{
}

void PhysicalEntity::createFromFile(std::string path, Sprite * sprite)
{
	clog << "Loading entity from file..." << endl;

	//load the xml file
	TiXmlDocument xmlDoc(path.c_str());
	if (!xmlDoc.LoadFile())
	{
		throw IOException(string("ERROR: Could not open XML file ") + path);
	}

	TiXmlElement *	objectElement   = 0;
	//TiXmlElement *	spriteElement   = 0;
	TiXmlElement *  scriptsElement  = 0;


	objectElement = xmlDoc.FirstChildElement("object");

	//check if it's a texture file
	if (!objectElement)
	{
		throw IOException(string("ERROR: Could not find XML element <object> "));
	}

	//type = objectElement->Attribute("type");
	entityName = objectElement->Attribute("name");
	entityType = objectElement->Attribute("type");

	//store the sprite name in StrData1
	//spriteElement = objectElement->FirstChildElement("sprite");

    //store the script functions to use
    scriptsElement = objectElement->FirstChildElement("scripts");
    if (scriptsElement)
    {
        TiXmlElement *  scriptElement = scriptsElement->FirstChildElement("script");
        string scriptType, scriptFunction;

        while (scriptElement)
        {
            scriptType = scriptElement->Attribute("type");
            scriptFunction = scriptElement->Attribute("function");

            if (!scriptType.length() || !scriptFunction.length())
            {
                clog << "Warning: Malformed object XML file in <script>: Missing type or function attribute (or they are at 0 length)." << endl;
                continue;
            }
            if (!scriptType.compare("init"))
            {
                initScripts.push_back(scriptFunction);
            }
            else if (!scriptType.compare("update"))
            {
                updateScripts.push_back(scriptFunction);
            }
            else if (!scriptType.compare("collision"))
            {
                collisionScripts.push_back(scriptFunction);
            }
            else if (!scriptType.compare("objcollision"))
            {
                objectCollisionScripts.push_back(scriptFunction);
            }

            scriptElement = scriptElement->NextSiblingElement("script");
        }
    }

	this->sprite = sprite;
}




bool PhysicalEntity::getDestructionFlag() const
{
	return destroyFlag;
}

void PhysicalEntity::setDestructionFlag(bool val)
{
	destroyFlag = val;
}

void PhysicalEntity::render(Renderer &renderer, Renderer::ObjectZ z)
{
	if (sprite)
	{
		sprite->setPosition(position.getX(), position.getY());
		sprite->render(renderer, z, 0, hFlip, vFlip);
	}
}

/*
    Position manipulation
*/
void PhysicalEntity::setPosition(double x, double y)
{
	position.set(x,y);
}

void PhysicalEntity::setPosition(Vector2<double> newPosition)
{
    position = newPosition;
}

void PhysicalEntity::setX(double x)
{
    position.setX(x);
}

void PhysicalEntity::setY(double y)
{
    position.setY(y);
}

Vector2<double> PhysicalEntity::getPosition() const
{
	return position;
}

double PhysicalEntity::getX()
{
    return position.getX();
}

double PhysicalEntity::getY()
{
    return position.getY();
}


/*
    Speed manipulation
*/


void PhysicalEntity::accellerate(double magnitude, Vector2<double> direction)
{
    //normalize the direction vector
    direction.normalize();

    velocity += direction * magnitude * frameTime;
}

void PhysicalEntity::accellerate(double magnitude, double xdir, double ydir)
{
    //normalize the direction vector
    Vector2<double> direction(xdir, ydir);
    direction.normalize();

    velocity += direction * magnitude * frameTime;
}


void PhysicalEntity::setMaxSpeed(double magnitude)
{

}

Vector2<double> PhysicalEntity::getVelocity() const
{
	return velocity;
}

void PhysicalEntity::setVelocity(const Vector2<double> newVel)
{
    velocity = newVel;
}

void PhysicalEntity::setVelocity(double x, double y)
{
    velocity.set(x,y);
}

double PhysicalEntity::getXVelocity()
{
    return velocity.getX();
}

double PhysicalEntity::getYVelocity()
{
    return velocity.getY();
}

void PhysicalEntity::setXVelocity(double x)
{
    velocity.setX(x);
}

void PhysicalEntity::setYVelocity(double y)
{
    velocity.setY(y);
}

double PhysicalEntity::getSpeed()
{
    return velocity.length();
}

//-----------------

void PhysicalEntity::setSprite(Sprite * sprite)
{
	this->sprite = sprite;
}

const Sprite * PhysicalEntity::getSprite() const
{
	return sprite;
}

/*
    Collision stuff
*/
//check collision between the entity and a target rect
bool PhysicalEntity::willCollide(PhysicalEntity &target, double frameTime, Vector2<double> &colPos1, Vector2<double> &colPos2)
{
	if (!canCollide || !sprite || !target.canCollide)
	{
		return false;
	}

    //store the current position and speed for both objects
    double dspeed1 = getSpeed() * frameTime;
    double dspeed2 = target.getSpeed() * frameTime;
    Vector2<double> currentPosition1 = position;
    Vector2<double> currentPosition2 = target.position;

    //get the speed of the fastest object (the one that will move furthest during the next frame)
    int dspeed;
    if (dspeed1 > dspeed2)
    {
        dspeed = (int)ceil(dspeed1);
    }
    else
    {
        dspeed = (int)ceil(dspeed2);
    }

    //find the delta velocity vector (largest increments should have length 1)
    Vector2<double> dvelocity1 = getVelocity() * frameTime / dspeed;
    Vector2<double> dvelocity2 = getVelocity() * frameTime / dspeed;
    Rect colRect1;
    Rect colRect2;
    //TODO: Optimization

    //check for collisions during the next frame
    for (int i = 0; i < dspeed; i++)
    {
        //get the collision rect for both objects
        colRect1 = getSprite()->getCollisionBox();
        colRect2 = target.getSprite()->getCollisionBox();

        colRect1.left += currentPosition1.getX();
        colRect1.right += currentPosition1.getX();
        colRect1.top += currentPosition1.getY();
        colRect1.bottom += currentPosition1.getY();

        colRect2.left += currentPosition2.getX();
        colRect2.right += currentPosition2.getX();
        colRect2.top += currentPosition2.getY();
        colRect2.bottom += currentPosition2.getY();

        //do the rects intersect?
        if (colRect1.left < colRect2.right && colRect1.right > colRect2.left && colRect1.top > colRect2.bottom && colRect1.bottom < colRect2.top)
        {
            collided = true;
            target.collided = true;
            colPos1 = currentPosition1;
            colPos2 = currentPosition2;
            return true;
        }

        //increment the position for a new check
        currentPosition1 -= dvelocity1;
        currentPosition2 -= dvelocity2;
    }

	return false;
}

bool PhysicalEntity::willCollideWithMap(Tilemap &tilemap, double frameTime)
{
	if (!canCollide || !sprite)
	{
		return false;
	}

	int dspeed = (int)ceil(getSpeed() * frameTime);

	Vector2<double> dvelocity = velocity * frameTime / dspeed;
    Vector2<double> currentPosition = position;
    Rect colRect;
    bool collided = false;

	for (int i = 0; i < dspeed; i++)
	{
	    //get the collision box of the sprite
		colRect = sprite->getCollisionBox();
		colRect.left += currentPosition.getX();
		colRect.right += currentPosition.getX();
		colRect.bottom += currentPosition.getY();
		colRect.top += currentPosition.getY();

		int tileSize = tilemap.getTileSize();

		Rect tilerect = tilemap.getTilesInRect(colRect.left - tileSize, colRect.top + tileSize, colRect.right + tileSize, colRect.bottom - tileSize);
		if (((tilerect.right - tilerect.left) > 0) && ((tilerect.top - tilerect.bottom) > 0))
		{
			for (int x = (int)tilerect.left; x <= (int)tilerect.right; x++)
			{
				for (int y = (int)tilerect.bottom; y <= (int)tilerect.top; y++)
				{
					//onMapCollision(tilemap, x * tileSize, y * tileSize + tileSize, x * tileSize + tileSize, y * tileSize, tilemap->getTileset()->getTile(tilemap->getTile(x,y,0)).block, frametime);
					if (tilemap.getTileset()->getTile(tilemap.getTile(x, y, 0)).block > 0)
					{
					    Rect tile;
					    tile.left = x*tileSize;
					    tile.right = x*tileSize + tileSize;
					    tile.top = y*tileSize + tileSize;
					    tile.bottom = y*tileSize;
					    if (colRect.left < tile.right && colRect.right > tile.left && colRect.top > tile.bottom && colRect.bottom < tile.top)
                        {
                            collided = true;
                            collidedTiles.push_back(TileCollisionData(Vector2<int>(x,y), Vector2<double>(currentPosition)));
                        }
					}
				}
			}
			break;
		}

		//increment the position by one step
		currentPosition -= dvelocity;
	}

    if (collidedTiles.size() > 0)
    {
        return true;
    }

	return false;
}

void PhysicalEntity::setCollision(bool value)
{
    canCollide = value;
}

void PhysicalEntity::update(double frameTime)
{
    this->frameTime = frameTime;
    position += velocity * frameTime;
    sprite->updateAnimation(frameTime);

    collided = false;
    collidedTiles.clear();
}

bool PhysicalEntity::playAnimation(std::string animationName, bool resetIfRunning, double animationSpeed)
{
    return sprite->playAnimation(animationName.c_str(), resetIfRunning, animationSpeed);
}



string PhysicalEntity::getName()
{
    return entityName;
}

string PhysicalEntity::getType()
{
    return entityType;
}


int PhysicalEntity::getHitboxBottom()
{
    Rect hitbox = sprite->getCollisionBox();

    return (int)hitbox.bottom;
}

int PhysicalEntity::getHitboxTop()
{
    Rect hitbox = sprite->getCollisionBox();

    return (int)hitbox.top;
}

int PhysicalEntity::getHitboxRight()
{
    Rect hitbox = sprite->getCollisionBox();

    return (int)hitbox.right;
}

int PhysicalEntity::getHitboxLeft()
{
    Rect hitbox = sprite->getCollisionBox();

    return (int)hitbox.left;
}

void PhysicalEntity::flipy(bool value)
{
    vFlip = value;
}

void PhysicalEntity::flipx(bool value)
{
    hFlip = value;
}



/*
void PhysicalEntity::setCollided(bool col)
{
    collided = col;
}

bool PhysicalEntity::hasCollided()
{
    return collided;
}

*/

