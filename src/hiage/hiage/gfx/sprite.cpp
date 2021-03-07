#include <iostream>

#include "sprite.h"
#include "../util/exceptions.h"
#include "../util/vector2.h"
#include "../core/resourcemanager.hpp"

using namespace std;
using namespace hiage;

SpriteAnimation::SpriteAnimation() : _currentFrame(0), _frameTimer(0), _velocitySpeedupFactor(0)
{
}

//add a new frame to the animation
void SpriteAnimation::addFrame(int x, int y, double delay, uint nextFrame, const BoundingBox<double>& colBox)
{
	_frames.push_back(AnimationFrame());
	auto& frame = _frames.back();
	frame.x = x;
	frame.y = y;
	frame.delay = delay;
	frame.nextFrame = nextFrame;

	//set the frame time to the delay of the first frame
	if (_frames.size() == 0)
	{
		_frameTimer = delay;
	}

	if (((colBox.right - colBox.left) > 0) && ((colBox.top - colBox.bottom) > 0))
	{
		frame.collisionBox = colBox;
	}
	else
	{
		frame.collisionBox = {0,0,0,0};
	}
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::runAnimation(double timefactor, double velocity)
{
	if (_frames.size() > 0)
	{
		_frameTimer -= (timefactor + abs(_velocitySpeedupFactor * velocity * timefactor));
		if (_frameTimer <= 0)
		{
			if (_currentFrame != _frames[_currentFrame].nextFrame)
			{
				_currentFrame = _frames[_currentFrame].nextFrame;
				_frameTimer = _frames[_currentFrame].delay;
			}
		}
	}
}

Sprite::Sprite() : _animationSpeed(1), _currentAnimation(0), _frameWidth(0), _frameHeight(0)
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

	this->_textureName = textureName;
	this->_frameWidth = frameWidth;
	this->_frameHeight = frameHeight;
}

void Sprite::render(const TextureManager& textureManager, Renderer &renderer, const Vector2<double>& position, ObjectZ z, float, bool hFlip, bool vFlip, float maxSize)
{
	//current sprite's position and dimensions
	double x = position.getX();
	double y = position.getY();

	auto& texture = textureManager.requestResourceRef(_textureName).resource;

	uint texwidth = texture.getWidth();
	uint texheight = texture.getHeight();

	double spriteWidth, spriteHeight;

    if (maxSize > 0 && (_frameWidth > maxSize || _frameHeight > maxSize))
    {
        if (_frameWidth > _frameHeight)
        {
            spriteWidth = maxSize;
            spriteHeight = maxSize * ((double)_frameHeight / (double)_frameWidth);
        }
        else
        {
            spriteHeight = maxSize;
            spriteWidth = maxSize * ((double)_frameWidth / (double)_frameHeight);
        }
    }
    else
    {
        spriteWidth = _frameWidth;
        spriteHeight = _frameHeight;
    }

	if ((_currentAnimation >= 0) && (_animations.size()) && (_currentAnimation <= _animations.size() - 1) && (_animations[_currentAnimation]._currentFrame >= 0))
	{
		//just a "shortcut" instead of using animations[currentAnimation]->frames[animations[currentAnimation]->currentFrame].stuff for every little thing
		auto&  frame = _animations[_currentAnimation]._frames[_animations[_currentAnimation]._currentFrame];

		//TODO: Implement rotation in renderer
		//glRotatef(rotation, 0.0f,0.0f,1.0f);

		double texLeft, texRight, texTop, texBottom;

		if (!hFlip)
		{
			texLeft = 1.0 / (double)texwidth * (double)frame.x;
			texRight = 1.0 / (double)texwidth * ((double)frame.x + (double)_frameWidth);
		}
		else
		{
			texLeft = 1.0 / (double)texwidth * ((double)frame.x + (double)_frameWidth);
			texRight = 1.0 / (double)texwidth * (double)frame.x;
		}

		if (!vFlip)
		{
			texTop = 1.0 / texheight * frame.y;
			texBottom = 1.0 / texheight * ((double)frame.y + _frameHeight);

		}
		else
		{
			texTop = 1.0 / texheight * ((double)frame.y + _frameHeight);
			texBottom = 1.0 / texheight * frame.y;
		}

		//render the sprite
		renderer.beginRender(z, &texture);
		renderer.addVertex(x, (double)y + spriteHeight, texLeft, texTop);
		renderer.addVertex((double)x + spriteWidth, (double)y + spriteHeight, texRight, texTop);
		renderer.addVertex((double)x + spriteWidth, y, texRight, texBottom);
		renderer.addVertex(x, y, texLeft, texBottom);
		renderer.endRender();
	}
	else
	{
		//render all frames if no animation is selected.
		renderer.beginRender(z, &texture);
		renderer.addVertex((double)x - (spriteWidth / 2), (double)y + (spriteHeight / 2), 0, 0);
		renderer.addVertex((double)x + (spriteWidth / 2), (double)y + (spriteHeight / 2), 0, 1);
		renderer.addVertex((double)x + (spriteWidth / 2), (double)y - (spriteHeight / 2), 1, 0);
		renderer.addVertex((double)x - (spriteWidth / 2), (double)y - (spriteHeight / 2), 1, 1);
		renderer.endRender();
	}
}

//add an animation to the sprite
unsigned int Sprite::addAnimation(const std::string& name, double velocitySpeedupFactor)
{
	clog << "Adding animation " << name << " to sprite...\n" << flush;
	_animations.push_back(SpriteAnimation());

	_animations.back().setName(name);
	_animations.back().setVelocitySpeedupFactor(velocitySpeedupFactor);
	if (_animations.size() == 1)
	{
		_currentAnimation = 0;
	}

	return (uint)_animations.size() - 1;
}

//add a frame to the animation
void Sprite::addFrame(uint animID, int x, int y, double delay, uint nextFrame, BoundingBox<double> colBox)
{
	//check for boundaries
	if (animID <= _animations.size()-1)
	{
		//add the frame
		_animations[animID].addFrame(x,y, delay, nextFrame, colBox);
	}
}

const std::string& Sprite::getCurrentAnimationName() const
{
	static const string empty = "";
	//check for boundaries
	if (_currentAnimation >= 0 && _currentAnimation <= _animations.size()-1)
	{
		//return the name
		return _animations.at(_currentAnimation).getName();
	}
	else
	{
		clog << "Warning: Sprite::getAnimationName(int) returns an empty string due to nonexistant id " << _currentAnimation << endl << flush;

		return empty;
	}
}

bool Sprite::playAnimation(const std::string& anim, bool resetIfRunning, double speed)
{
	for (uint i = 0; i < _animations.size(); i++)
	{
		if (!_animations[i].getName().compare(anim))
		{
			if (resetIfRunning)
			{
				if (_currentAnimation == i)
				{
					_animations[i].reset();
				}
			}

			_currentAnimation = i;
			_animationSpeed = speed;;

			return true;
		}
	}

	return false;
}

void Sprite::updateAnimation(double timefactor, double velocity)
{
	if ((_currentAnimation >= 0) && (_currentAnimation <= _animations.size()-1))
	{
		_animations[_currentAnimation].runAnimation(timefactor * _animationSpeed, velocity);
	}
}

Sprite & Sprite::operator=(Sprite sprite)
{
	//clear any previous animations
	for (uint i = 0; i < _animations.size(); i++)
	{
		_animations.erase(_animations.begin()+i);
		if (i > 0)
		{
			i--;
		}
	}

	for (uint i = 0; i < sprite._animations.size(); i++)
	{
		_animations.push_back(sprite._animations[i]);
	}

	return *this;
}
