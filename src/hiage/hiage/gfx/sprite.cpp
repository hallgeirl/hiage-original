#include <iostream>

#include "sprite.h"
#include "../util/exceptions.h"
#include "../util/vector2.h"

using namespace std;
using namespace hiage;

SpriteAnimation::SpriteAnimation() : currentFrame(0), frameTimer(0)
{
}

//add a new frame to the animation
void SpriteAnimation::addFrame(int x, int y, double delay, uint nextFrame, Rect colBox)
{
	Frame *fr = new Frame;
	fr->x = x;
	fr->y = y;
	fr->delay = delay;
	fr->nextFrame = nextFrame;

	//set the frame time to the delay of the first frame
	if (frames.size() == 0)
	{
		frameTimer = delay;
	}

	if (((colBox.right - colBox.left) > 0) && ((colBox.top - colBox.bottom) > 0))
	{
		fr->collisionBox = colBox;
	}
	else
	{
		memset(&fr->collisionBox, 0, sizeof(Rect));
	}

	frames.push_back(fr);
}

SpriteAnimation::~SpriteAnimation()
{
	for (uint i = 0; i < frames.size(); i++)
	{
		delete frames[i];
	}
}

void SpriteAnimation::runAnimation(double timefactor)
{
	if (frames.size() > 0)
	{
		frameTimer -= timefactor;
		if (frameTimer <= 0)
		{
			if (currentFrame != frames[currentFrame]->nextFrame)
			{
				currentFrame = frames[currentFrame]->nextFrame;
				frameTimer = frames[currentFrame]->delay;
			}
		}
	}
}

Sprite::Sprite() : animationSpeed(1), currentAnimation(0), frameWidth(0), frameHeight(0), texture(nullptr)
{
}

Sprite::~Sprite()
{
}

//create the sprite
void Sprite::create(Texture * texture, int frameWidth, int frameHeight)
{
	if (!texture || frameWidth <= 0 || frameHeight <= 0)
	{
		throw Exception("ERROR: Sprite::create(): Invalid frame dimensions.");
	}

	this->texture = texture;
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
}

void Sprite::render(Renderer &renderer, const Vector2<double>& position, ObjectZ z, float rotation, bool hFlip, bool vFlip, float maxSize)
{
	//current sprite's position and dimensions
	double x = position.getX();
	double y = position.getY();

	uint texwidth = texture->getWidth();
	uint texheight = texture->getHeight();

	double spriteWidth, spriteHeight;

    if (maxSize > 0 && (frameWidth > maxSize || frameHeight > maxSize))
    {
        if (frameWidth > frameHeight)
        {
            spriteWidth = maxSize;
            spriteHeight = maxSize * ((double)frameHeight / (double)frameWidth);
        }
        else
        {
            spriteHeight = maxSize;
            spriteWidth = maxSize * ((double)frameWidth / (double)frameHeight);
        }
    }
    else
    {
        spriteWidth = frameWidth;
        spriteHeight = frameHeight;
    }

	if ((currentAnimation >= 0) && (animations.size()) && (currentAnimation <= animations.size() - 1) && (animations[currentAnimation]->currentFrame >= 0))
	{
		//just a "shortcut" instead of using animations[currentAnimation]->frames[animations[currentAnimation]->currentFrame].stuff for every little thing
		SpriteAnimation::Frame * frame =
			animations[currentAnimation]->frames[animations[currentAnimation]->currentFrame];

		//TODO: Implement rotation in renderer
		//glRotatef(rotation, 0.0f,0.0f,1.0f);

		double texLeft, texRight, texTop, texBottom;

		if (!hFlip)
		{
			texLeft = 1.0 / (double)texwidth * (double)frame->x;
			texRight = 1.0 / (double)texwidth * ((double)frame->x + (double)frameWidth);
		}
		else
		{
			texLeft = 1.0 / (double)texwidth * ((double)frame->x + (double)frameWidth);
			texRight = 1.0 / (double)texwidth * (double)frame->x;
		}

		if (!vFlip)
		{
			texTop = 1.0 / texheight * frame->y;
			texBottom = 1.0 / texheight * ((double)frame->y + frameHeight);

		}
		else
		{
			texTop = 1.0 / texheight * ((double)frame->y + frameHeight);
			texBottom = 1.0 / texheight * frame->y;
		}

		//render the sprite
		renderer.beginRender(z, texture);
		renderer.addVertex(x, (double)y + spriteHeight, texLeft, texTop);
		renderer.addVertex((double)x + spriteWidth, (double)y + spriteHeight, texRight, texTop);
		renderer.addVertex((double)x + spriteWidth, y, texRight, texBottom);
		renderer.addVertex(x, y, texLeft, texBottom);
		renderer.endRender();
	}
	else
	{
		//render all frames if no animation is selected.
		renderer.beginRender(z, texture);
		renderer.addVertex((double)x - (spriteWidth / 2), (double)y + (spriteHeight / 2), 0, 0);
		renderer.addVertex((double)x + (spriteWidth / 2), (double)y + (spriteHeight / 2), 0, 1);
		renderer.addVertex((double)x + (spriteWidth / 2), (double)y - (spriteHeight / 2), 1, 0);
		renderer.addVertex((double)x - (spriteWidth / 2), (double)y - (spriteHeight / 2), 1, 1);
		renderer.endRender();
	}
}

//add an animation to the sprite
unsigned int Sprite::addAnimation(const char * name)
{
	clog << "Adding animation " << name << " to sprite...\n" << flush;
	SpriteAnimation * animation = new SpriteAnimation;
	animations.push_back(animation);

	animations[animations.size()-1]->setName(name);

	if (animations.size() == 1)
	{
		currentAnimation = 0;
	}

	return (uint)animations.size() - 1;
}

//add a frame to the animation
void Sprite::addFrame(uint animID, int x, int y, double delay, uint nextFrame, Rect colBox)
{
	//check for boundaries
	if (animID <= animations.size()-1)
	{
		//add the frame
		animations[animID]->addFrame(x,y, delay, nextFrame, colBox);
	}
}

const std::string Sprite::getAnimationName(uint animID)
{
	//check for boundaries
	if (animID <= animations.size()-1)
	{
		//return the name
		return animations[animID]->getName();
	}
	else
	{
		clog << "Warning: Sprite::getAnimationName(int) returns an empty string due to nonexistant id " << animID << endl << flush;

		return "";
	}
}

bool Sprite::playAnimation(const char * anim, bool resetIfRunning, double speed)
{
	for (uint i = 0; i < animations.size(); i++)
	{
		if (!animations[i]->getName().compare(anim))
		{
			if (resetIfRunning)
			{
				if (currentAnimation == i)
				{
					animations[i]->reset();
				}
			}

			currentAnimation = i;
			animationSpeed = speed;

			return true;
		}
	}

	return false;
}

void Sprite::updateAnimation(double timefactor)
{
	if ((currentAnimation >= 0) && (currentAnimation <= animations.size()-1))
	{
		animations[currentAnimation]->runAnimation(timefactor * animationSpeed);
	}
}

Sprite & Sprite::operator=(Sprite sprite)
{
	//clear any previous animations
	for (uint i = 0; i < animations.size(); i++)
	{

		delete animations[i];
		animations.erase(animations.begin()+i);
		if (i > 0)
		{
			i--;
		}
	}

	for (uint i = 0; i < sprite.animations.size(); i++)
	{
		SpriteAnimation * anim = new SpriteAnimation;
		*anim = *sprite.animations[i];
		animations.push_back(anim);
	}

	return *this;
}

Rect Sprite::getCollisionBox() const
{
	if ((currentAnimation >= 0) && (currentAnimation < animations.size()))
	{
		return animations[currentAnimation]->getCurrentFrame()->collisionBox;
	}
	else
	{
		Rect r(0,0,0,0);
		return r;
	}
}
