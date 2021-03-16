#include "spritecontroller.hpp"
#include "../core/game.hpp"

using namespace hiage;

void SpriteController::render(const Sprite& sprite, const Vector2<double>& position, ObjectZ z, float, bool hFlip, bool vFlip, float maxSize) const
{
    auto& renderer = _game.getDisplay().getRenderer();
    auto& texManager = _game.getTextureManager();

    auto& texture = texManager.requestResourceRef(sprite.textureName).resource;

	//current sprite's position and dimensions
	double x = position.getX();
	double y = position.getY();

	uint texwidth = texture.getWidth();
	uint texheight = texture.getHeight();

	double spriteWidth, spriteHeight;

    if (maxSize > 0 && (sprite.frameWidth > maxSize || sprite.frameHeight > maxSize))
    {
        if (sprite.frameWidth > sprite.frameHeight)
        {
            spriteWidth = maxSize;
            spriteHeight = maxSize * ((double)sprite.frameHeight / (double)sprite.frameWidth);
        }
        else
        {
            spriteHeight = maxSize;
            spriteWidth = maxSize * ((double)sprite.frameWidth / (double)sprite.frameHeight);
        }
    }
    else
    {
        spriteWidth = sprite.frameWidth;
        spriteHeight = sprite.frameHeight;
    }


	if ((sprite.currentAnimation >= 0) && (sprite.animations.size()) && (sprite.currentAnimation <= sprite.animations.size() - 1) && (sprite.animations[sprite.currentAnimation].currentFrame >= 0))
	{
		//just a "shortcut" instead of using animations[currentAnimation]->frames[animations[currentAnimation]->currentFrame].stuff for every little thing
		auto&  frame = sprite.animations[sprite.currentAnimation].frames[sprite.animations[sprite.currentAnimation].currentFrame];

		//TODO: Implement rotation in renderer
		//glRotatef(rotation, 0.0f,0.0f,1.0f);

		double texLeft, texRight, texTop, texBottom;

		if (!hFlip)
		{
			texLeft = 1.0 / (double)texwidth * (double)frame.x;
			texRight = 1.0 / (double)texwidth * ((double)frame.x + (double)sprite.frameWidth);
		}
		else
		{
			texLeft = 1.0 / (double)texwidth * ((double)frame.x + (double)sprite.frameWidth);
			texRight = 1.0 / (double)texwidth * (double)frame.x;
		}

		if (!vFlip)
		{
			texTop = 1.0 / texheight * frame.y;
			texBottom = 1.0 / texheight * ((double)frame.y + sprite.frameHeight);

		}
		else
		{
			texTop = 1.0 / texheight * ((double)frame.y + sprite.frameHeight);
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