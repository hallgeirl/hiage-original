#pragma once



#include "../core/typedefs.h"
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
	class __IMPORTEXPORT SpriteAnimation
	{
		//TODO: Solve this so friend class isn't needed
		friend class Sprite;	//give the sprite class full access to the animation

	private:
		int	_currentFrame;	//current frame that is showing
		double _frameTimer;	//keeps track of when to go to the next frame
		double _velocitySpeedupFactor;

		std::vector<AnimationFrame> _frames;
		std::string _animationName;

	public:
		SpriteAnimation();
		~SpriteAnimation();

		void setName(const std::string& animName) {_animationName = animName;}
		void setVelocitySpeedupFactor(double factor) { _velocitySpeedupFactor = factor; }
		const std::string& getName() const { return _animationName; }
		void addFrame(int x, int y, double delay, uint nextFrame, const BoundingBox<double>& colBox);

		void runAnimation(double timeFactor, double velocity);
		void reset() { _frameTimer = 0; _currentFrame = 0; }

		const AnimationFrame& getCurrentFrame() { return _frames[_currentFrame]; }

		//check if the animation is playing. if the next frame is the same as this one, it's not.
		bool isPlaying()
		{
			return (_frameTimer > 0);
		}
	};

	//class to handle sprites
	class __IMPORTEXPORT Sprite
	{
	private:
		//pointer to the texture that is used
		//const Texture* _texture;
		std::string _textureName;

		//frame dimensions
		int _frameWidth;
		int _frameHeight;

		//animation
		double _animationSpeed;
		uint _currentAnimation;
		std::vector<SpriteAnimation> _animations;

	public:
		Sprite();
		~Sprite();

		void create(const std::string& textureName, int frameWidth, int frameHeight);

		void render(const TextureManager& textureManager, Renderer &renderer, const Vector2<double>& position, ObjectZ z, float rotation = 0, bool hFlip = false, bool vFlip = false, float maxSize = 0);

		uint addAnimation(const std::string&, double velocitySpeedupFactor);
		void addFrame(uint animID, int x, int y, double delay, uint nextFrame, BoundingBox<double> colBox);
		const std::string& getCurrentAnimationName() const;
		void updateAnimation(double timefactor, double velocity);
		void playAnimation(int anim, double speed = 1) { _currentAnimation = anim; _animationSpeed = speed; }
		bool playAnimation(const std::string& anim, bool resetIfRunning = true, double speed = 1);


		bool animationIsPlaying() { return (_animations[_currentAnimation].isPlaying()); }

		int getWidth() const { return _frameWidth; }
		int getHeight() const { return _frameHeight; }

		Sprite & operator=(Sprite);
	};
}
