#pragma once



#include "../core/typedefs.h"
#include "texture.h"
#include "renderer.h"
#include "../util/vector2.h"
#include <string>
#include <vector>

namespace hiage
{
	//class to handle animations (keeping track of all frames, when to go to next frame etc.)
	class __IMPORTEXPORT SpriteAnimation
	{
		//TODO: Solve this so friend class isn't needed
		friend class Sprite;	//give the sprite class full access to the animation

		//frame structure - simple enough
		struct Frame
		{
			int x,y;
			double delay;
			int nextFrame;
			BoundingBox<double> collisionBox;
		};

	private:
		int	currentFrame;	//current frame that is showing
		double frameTimer;	//keeps track of when to go to the next frame

		//C_COM_DynArray<Frame*> m_Frames;
		std::vector<Frame*> frames;
		std::string animationName;

	public:
		SpriteAnimation();
		~SpriteAnimation();

		void setName(const std::string animName) {animationName = animName;}
		const std::string& getName() const { return animationName; }
		void addFrame(int x, int y, double delay, uint nextFrame, BoundingBox<double> colBox);
		void runAnimation(double timeFactor);
		void reset() { frameTimer = 0; currentFrame = 0; }

		Frame * getCurrentFrame() { return frames[currentFrame]; }

		//check if the animation is playing. if the next frame is the same as this one, it's not.
		bool isPlaying()
		{
			return (frameTimer > 0);
		}
	};

	//class to handle sprites
	class __IMPORTEXPORT Sprite
	{
	private:
		//pointer to the texture that is used
		Texture * texture;

		//frame dimensions
		int frameWidth;
		int frameHeight;

		//animation
		double animationSpeed;
		uint currentAnimation;
		std::vector<SpriteAnimation *> animations;

	public:
		Sprite();
		~Sprite();

		void create(Texture * texture, int frameWidth, int frameHeight);

		void render(Renderer &renderer, const Vector2<double>& position, ObjectZ z, float rotation = 0, bool hFlip = false, bool vFlip = false, float maxSize = 0);

		uint addAnimation(const std::string&);
		void addFrame(uint animID, int x, int y, double delay, uint nextFrame, BoundingBox<double> colBox);
		const std::string& getCurrentAnimationName() const;
		void updateAnimation(double timefactor);
		void playAnimation(int anim, double speed = 1) { currentAnimation = anim; animationSpeed = speed; }
		bool playAnimation(const std::string& anim, bool resetIfRunning = true, double speed = 1);


		bool animationIsPlaying() { return (animations[currentAnimation]->isPlaying()); }

		int getWidth() const { return frameWidth; }
		int getHeight() const { return frameHeight; }

		Sprite & operator=(Sprite);
	};
}
