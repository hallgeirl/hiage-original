
#include "sprite.h"
#include "../util/exceptions.h"
#include "../util/vector2.h"
#include "../core/resourcemanager.hpp"

#include <iostream>

using namespace std;
using namespace hiage;

SpriteAnimation::SpriteAnimation() : currentFrame(0), frameTimer(0), velocitySpeedupFactor(0)
{
}

//add a new frame to the animation
void SpriteAnimation::addFrame(int x, int y, double delay, uint nextFrame)
{
	frames.push_back(AnimationFrame());
	auto& frame = frames.back();
	frame.x = x;
	frame.y = y;
	frame.delay = delay;
	frame.nextFrame = nextFrame;

	//set the frame time to the delay of the first frame
	if (frames.size() == 0)
	{
		frameTimer = delay;
	}
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::runAnimation(double timefactor, double velocity)
{
	if (frames.size() > 0)
	{
		frameTimer -= (timefactor + abs(velocitySpeedupFactor * velocity * timefactor));
		if (frameTimer <= 0)
		{
			if (currentFrame != frames[currentFrame].nextFrame)
			{
				currentFrame = frames[currentFrame].nextFrame;
				frameTimer = frames[currentFrame].delay;
			}
		}
	}
}

Sprite::Sprite() : animationSpeed(1), currentAnimation(0), frameWidth(0), frameHeight(0)
{
}

Sprite::~Sprite()
{
}

//create the sprite
void Sprite::create(const std::string& textureName, int frameWidth, int frameHeight)
{
	if (frameWidth <= 0 || frameHeight <= 0)
	{
		throw Exception("ERROR: Sprite::create(): Invalid frame dimensions.");
	}

	this->textureName = textureName;
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
}

//add an animation to the sprite
unsigned int Sprite::addAnimation(const std::string& name, double velocitySpeedupFactor)
{
	clog << "Adding animation " << name << " to sprite...\n" << flush;
	animations.push_back(SpriteAnimation());

	animations.back().setName(name);
	animations.back().setVelocitySpeedupFactor(velocitySpeedupFactor);
	if (animations.size() == 1)
	{
		currentAnimation = 0;
	}

	return (uint)animations.size() - 1;
}

//add a frame to the animation
void Sprite::addFrame(uint animID, int x, int y, double delay, uint nextFrame)
{
	//check for boundaries
	if (animID <= animations.size()-1)
	{
		//add the frame
		animations[animID].addFrame(x,y, delay, nextFrame);
	}
}

const std::string& Sprite::getCurrentAnimationName() const
{
	static const string empty = "";
	//check for boundaries
	if (currentAnimation >= 0 && currentAnimation <= animations.size()-1)
	{
		//return the name
		return animations.at(currentAnimation).getName();
	}
	else
	{
		clog << "Warning: Sprite::getAnimationName(int) returns an empty string due to nonexistant id " << currentAnimation << endl << flush;

		return empty;
	}
}

bool Sprite::playAnimation(const std::string& anim, bool resetIfRunning, double speed)
{
	for (uint i = 0; i < animations.size(); i++)
	{
		if (!animations[i].getName().compare(anim))
		{
			if (resetIfRunning)
			{
				if (currentAnimation == i)
				{
					animations[i].reset();
				}
			}

			currentAnimation = i;
			animationSpeed = speed;;

			return true;
		}
	}

	return false;
}

void Sprite::updateAnimation(double timefactor, double velocity)
{
	if ((currentAnimation >= 0) && (currentAnimation <= animations.size()-1))
	{
		animations[currentAnimation].runAnimation(timefactor * animationSpeed, velocity);
	}
}
