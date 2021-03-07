/*#include "spritecontroller.hpp"

using namespace hiage;

void SpriteController::render(const Sprite& sprite, Renderer &renderer, const Vector2<double>& position, ObjectZ z, float, bool hFlip, bool vFlip, float maxSize)
{
	//current sprite's position and dimensions
	double x = position.getX();
	double y = position.getY();

	uint texwidth = _texture->getWidth();
	uint texheight = _texture->getHeight();

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
		renderer.beginRender(z, _texture);
		renderer.addVertex(x, (double)y + spriteHeight, texLeft, texTop);
		renderer.addVertex((double)x + spriteWidth, (double)y + spriteHeight, texRight, texTop);
		renderer.addVertex((double)x + spriteWidth, y, texRight, texBottom);
		renderer.addVertex(x, y, texLeft, texBottom);
		renderer.endRender();
	}
	else
	{
		//render all frames if no animation is selected.
		renderer.beginRender(z, _texture);
		renderer.addVertex((double)x - (spriteWidth / 2), (double)y + (spriteHeight / 2), 0, 0);
		renderer.addVertex((double)x + (spriteWidth / 2), (double)y + (spriteHeight / 2), 0, 1);
		renderer.addVertex((double)x + (spriteWidth / 2), (double)y - (spriteHeight / 2), 1, 0);
		renderer.addVertex((double)x - (spriteWidth / 2), (double)y - (spriteHeight / 2), 1, 1);
		renderer.endRender();
	}
}*/