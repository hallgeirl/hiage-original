#pragma once

#include "../core/typedefs.h"
#include "../core/boundingbox.hpp"

#include "texture.h"
#include "renderer.h"
#include "../util/vector2.h"
#include <string>
#include <vector>

namespace hiage
{
	class TextureManager;

	// Metadata for a frame in the animation
	struct AnimationFrame
	{
		int x,y;
		double delay;
		int nextFrame;
		BoundingBox<double> collisionBox;
	};

	//class to handle animations (keeping track of all frames, when to go to next frame etc.)
	struct __IMPORTEXPORT SpriteAnimation
	{
		int	currentFrame;	//current frame that is showing
		double frameTimer;	//keeps track of when to go to the next frame
		double velocitySpeedupFactor;

		std::vector<AnimationFrame> frames;
		std::string animationName;

	public:
		SpriteAnimation();
		~SpriteAnimation();

		void setName(const std::string& animName) {animationName = animName;}
		void setVelocitySpeedupFactor(double factor) { velocitySpeedupFactor = factor; }
		const std::string& getName() const { return animationName; }
		void addFrame(int x, int y, double delay, uint nextFrame);

		void runAnimation(double timeFactor, double velocity);
		void reset() { frameTimer = 0; currentFrame = 0; }

		const AnimationFrame& getCurrentFrame() { return frames[currentFrame]; }

		//check if the animation is playing. if the next frame is the same as this one, it's not.
		bool isPlaying()
		{
			return (frameTimer > 0);
		}
	};

	//class to handle sprites
	struct __IMPORTEXPORT Sprite
	{
		//pointer to the texture that is used
		std::string textureName;

		//frame dimensions
		int frameWidth;
		int frameHeight;

		//animation
		double animationSpeed;
		uint currentAnimation;
		std::vector<SpriteAnimation> animations;

	public:
		Sprite();
		~Sprite();

		void create(const std::string& textureName, int frameWidth, int frameHeight);

		uint addAnimation(const std::string&, double velocitySpeedupFactor);
		void addFrame(uint animID, int x, int y, double delay, uint nextFrame);
		const std::string& getCurrentAnimationName() const;
		void updateAnimation(double timefactor, double velocity);
		void playAnimation(int anim, double speed = 1) { currentAnimation = anim; animationSpeed = speed; }
		bool playAnimation(const std::string& anim, bool resetIfRunning = true, double speed = 1);


		bool animationIsPlaying() { return (animations[currentAnimation].isPlaying()); }

		int getWidth() const { return frameWidth; }
		int getHeight() const { return frameHeight; }
	};
}
